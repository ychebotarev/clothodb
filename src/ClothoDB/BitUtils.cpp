#include <intrin.h>
#include "BitUtils.h"

namespace incolun{
namespace clothodb{

const int mask[] = 
{ 
    ((1 << 0) - 1),
    ((1 << 1) - 1),
    ((1 << 2) - 1),
    ((1 << 3) - 1),
    ((1 << 4) - 1),
    ((1 << 5) - 1),
    ((1 << 6) - 1),
    ((1 << 7) - 1)
};
static unsigned char bitPositionMask[] = 
{ 
    1 , 
    1 << 1,
    1 << 2,
    1 << 3,
    1 << 4,
    1 << 5,
    1 << 6,
    1 << 7
};

//top = (n >> (8-bitsToRead)) & ((1 << bitsToRead) - 1)
uint8_t BitUtils::ReadHiBits(uint8_t byte, uint8_t bitsToRead)
{
    uint8_t shift = 8 - bitsToRead;
    uint8_t value = (byte >> shift) & mask[bitsToRead];
    return value;
}

//bottom = n & ((1 << bitsToRead) - 1)
uint8_t BitUtils::ReadLoBits(uint8_t byte, uint8_t bitsToRead)
{
    uint8_t value = byte & mask[bitsToRead];
    return value;
}

uint8_t BitUtils::ReadMidBits(uint8_t byte, uint8_t start, uint8_t bitsToRead)
{
    return ReadHiBits(byte << start, bitsToRead);
}

void BitUtils::WriteBits32(
    std::vector<uint8_t>& buffer,
    uint32_t bitPosision,
    uint32_t value,
    uint8_t bitsToStore)
{
    WriteBits(buffer, bitPosision, value, bitsToStore);
}

void BitUtils::WriteBits64(
    std::vector<uint8_t>& buffer,
    uint32_t bitPosision,
    uint64_t value,
    uint8_t bitsToStore)
{
    WriteBits(buffer, bitPosision, value, bitsToStore);
}

template <typename T>
void BitUtils::WriteBits(
    std::vector<uint8_t>& buffer,
    uint32_t bitPosision,
    T value,
    uint8_t bitsToStore)
{
    int bufferLength = bitPosision >> 3;
    uint8_t availableBits = (bitPosision & 0x7) ? (8 - (bitPosision & 0x7)) : 0;

    //check if everything fits into current byte
    if (bitsToStore <= availableBits) 
    {
        buffer[bufferLength] +=  (uint8_t)(value << (availableBits - bitsToStore));
        return;
    }

    //fill current byte
    if (availableBits > 0) 
    {
        auto valueToAdd = value >> (bitsToStore - availableBits);
        buffer[bufferLength++] += (uint8_t)valueToAdd;
        bitsToStore -= availableBits;
    }

    //add whole byte
    while (bitsToStore >= 8)
    {
        char singleChar = (value >> (bitsToStore - 8)) & 0xFF;
        buffer[bufferLength++]= singleChar;
        bitsToStore -= 8;
    }

    //add remainder
    if (bitsToStore != 0) 
    {
        char singleChar = (value & ((1 << bitsToStore) - 1)) << (8 - bitsToStore);
        buffer[bufferLength] = singleChar;
    }
}

uint32_t BitUtils::ReadBit(
    const std::vector<uint8_t>& buffer,
    uint32_t bitPosision)
{
    int bufferLength = bitPosision >> 3;
    bitPosision &= 0x7;
    bitPosision = 7 - bitPosision;
    auto ch = buffer[bufferLength];
    return ((ch & bitPositionMask[bitPosision]) != 0);
}

uint32_t BitUtils::ReadBits32(
    const std::vector<uint8_t>& buffer,
    uint32_t bitPosision,
    uint8_t bitsToRead)
{
    return ReadBits<uint32_t>(buffer, bitPosision, bitsToRead);
}

uint64_t BitUtils::ReadBits64(
    const std::vector<uint8_t>& buffer,
    uint32_t bitPosision,
    uint8_t bitsToRead)
{
    return ReadBits<uint64_t>(buffer, bitPosision, bitsToRead);
}

template <typename T>
T BitUtils::ReadBits(
    const std::vector<uint8_t>& buffer,
    uint32_t bitPosision,
    uint8_t bitsToRead)
{
    int bufferLength = bitPosision >> 3;
    uint8_t availableBits = (bitPosision & 0x7) ? (8 - (bitPosision & 0x7)) : 0;

    T value = 0;
    if (bitsToRead < availableBits) 
    {
        return ReadMidBits(buffer[bufferLength], bitPosision, bitsToRead);
    }

    if (availableBits > 0)
    {
        value += ReadLoBits(buffer[bufferLength++], availableBits);
        bitsToRead -= availableBits;
    }

    while (bitsToRead >= 8)
    {
        value = value << 8;
        value += buffer[bufferLength++];
        bitsToRead -= 8;
    }

    if (bitsToRead > 0)
    {
        value = value << bitsToRead;
        value += ReadHiBits(buffer[bufferLength], bitsToRead);
    }

    return value;
}

}}//end of incolun::clothdb namespace