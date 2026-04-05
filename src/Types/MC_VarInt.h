#pragma once
#include <cstdio>
#include<cstdint>
#include <vector>

#define SEGMENT_BITS 0x7F
#define CONTINUE_BIT 0x80

#define VAR_INT_INVALID -1

struct MC_VarInt {

    int value;
    int storageSize = 0;

    void set(const uint8_t* buf);
    
    void set(int val);

    // https://minecraft.wiki/w/Java_Edition_protocol/Packets#VarInt_and_VarLong
    static int readVarInt(const uint8_t* buf, int* bytesRead = nullptr);

    static void writeVarInt(int value, std::vector<uint8_t>& data);

    MC_VarInt() = default;
    MC_VarInt(const uint8_t* buf);
    MC_VarInt(int val);

    MC_VarInt& operator=(const int& val);

    MC_VarInt& operator=(const MC_VarInt& vInt);

    MC_VarInt& operator=(const uint8_t* buf);

};