#include"PacketManager.h"
#include<Windows.h>

#include "../Config.h"
#include"../Types/MC_VarInt.h"

void PacketManager::processInHandshake(uint8_t* buf, int& len) {
#ifndef MODIFY_HANDSHAKE
    return;
#endif
    char* old = new char[len];
    memcpy(old, buf, len);

    int readOffset = 0;

    MC_VarInt packetLen(buf);
    readOffset += packetLen.storageSize;
    
    if (len < packetLen.value) {
        MessageBox(0, TEXT("Bad packet size. Please reconnect."), MB_OK, 0);
        delete[] old;
        return;
    }

    MC_VarInt packetID(buf + readOffset);
    readOffset += packetID.storageSize;

    if (packetID.value != 0x00) {
        MessageBox(0, TEXT("Bad handshake. Please reconnect."), MB_OK, 0);
        delete[] old;
        return;
    }

    MC_VarInt protocolVersion(buf + readOffset);
    readOffset += protocolVersion.storageSize;

    MC_VarInt addresLen(buf + readOffset);
    readOffset += addresLen.storageSize;
    readOffset += addresLen.value;

    int dif = strlen(CURRENT_IP) - addresLen.value;
    int newLen = len + dif;
    std::vector<uint8_t> newBuf;
    newBuf.reserve(len + dif);

    uint8_t* portAsU8 = (uint8_t*)buf + readOffset;

    unsigned short port = (portAsU8[0] << 8) | portAsU8[1]; 
    readOffset += 2;

    MC_VarInt intent(buf + readOffset);
    readOffset += intent.storageSize;

    int writeOffset = 0;
    MC_VarInt::writeVarInt(dif + packetLen.value, newBuf);
    newBuf.push_back(0); 
    MC_VarInt::writeVarInt(protocolVersion.value, newBuf);
    MC_VarInt::writeVarInt(addresLen.value + dif, newBuf); 
    newBuf.insert(newBuf.end(), CURRENT_IP, CURRENT_IP + strlen(CURRENT_IP)); // new ip
    newBuf.push_back(portAsU8[0]);
    newBuf.push_back(portAsU8[1]);
    MC_VarInt::writeVarInt(intent.value, newBuf);

    newBuf.insert(newBuf.end(), old + packetLen.value + packetLen.storageSize, old + len);

    memcpy(buf, newBuf.data(), newLen);

    len += dif;

    delete[] old;

    printf("Handshake packet modified. New IP: %s\n", CURRENT_IP);
    gameState = (GameState)intent.value;
}

void PacketManager::processPacket(uint8_t* buf, int& len) {
    switch (gameState) {
    case GameState::HANDSHAKING:
        processInHandshake(buf, len);
    default:
        break;
    }
}
