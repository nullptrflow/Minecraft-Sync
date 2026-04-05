#include"MC_VarInt.h"

void MC_VarInt::set(const uint8_t* buf) {
    value = readVarInt(buf, &storageSize);
}

void MC_VarInt::set(int val) {
    value = val;

    storageSize = 1;
    int bit = 31;
    for (; bit >= 0; bit--) {
        if (((val >> bit) & 1) == 1) { // 1 ���������
            storageSize = (bit + 1) / 7 + ((bit + 1) % 7 == 0 ? 0 : 1);
            break;
        }
    }
}

int MC_VarInt::readVarInt(const uint8_t* buf, int* bytesRead) {

    // LE
    int value = 0;
    int bytes = 0;
    uint8_t currentByte;
    const uint8_t* originalBuf = reinterpret_cast<const uint8_t*>(buf);

    while (true) {
        currentByte = *originalBuf;
        originalBuf++;
        bytes++;

        value |= (currentByte & SEGMENT_BITS) << (7 * (bytes - 1));

        if ((currentByte & CONTINUE_BIT) == 0) break;

        if (bytes >= 5) {
            printf("ERROR: VarInt too long!\n");
            if (bytesRead) *bytesRead = 0;
            return -1;
        }
    }

    if (bytesRead) *bytesRead = bytes;
    return value;
}

void MC_VarInt::writeVarInt(int value, std::vector<uint8_t>& data) {

    while ((value & 0xFFFFFF80) != 0) {
        data.push_back((value & 0x7F) | 0x80);
        value >>= 7;
    }

    data.push_back(value);
}

MC_VarInt::MC_VarInt(const uint8_t* buf) {
    this->set(buf);
}
MC_VarInt::MC_VarInt(int val) {
    this->set(val);
}

MC_VarInt& MC_VarInt::operator=(const int& val) {
    this->set(val);
    return *this;
}

MC_VarInt& MC_VarInt::operator=(const MC_VarInt& vInt) {
    this->storageSize = vInt.storageSize;
    this->value = vInt.value;
    return *this;
}

MC_VarInt& MC_VarInt::operator=(const uint8_t* buf) {
    value = readVarInt(buf, &storageSize);
    return *this;
}
