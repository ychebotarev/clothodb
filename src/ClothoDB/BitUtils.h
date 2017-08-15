#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <intrin.h>

namespace incolun{
namespace clothodb{

class BitUtils
{
public:
    // Write specified number of bits from value into byte buffer
    // bitPosision indicates starting position in bits
    static void WriteBits32(
        std::vector<uint8_t>& buffer,
        uint32_t bitPosision,
        uint32_t value,
        uint8_t bitsToStore);

    static void WriteBits64(
        std::vector<uint8_t>& buffer,
        uint32_t bitPosision,
        uint64_t value,
        uint8_t bitsToStore);

    static uint32_t ReadBit(
        const std::vector<uint8_t>& buffer,
        uint32_t bitPosision);

    static uint32_t ReadBits32(
        const std::vector<uint8_t>& buffer,
        uint32_t bitPosision,
        uint8_t bitsToRead);

    static uint64_t ReadBits64(
        const std::vector<uint8_t>& buffer,
        uint32_t bitPosision,
        uint8_t bitsToRead);

    static uint8_t ReadHiBits(uint8_t byte, uint8_t bitsToRead);
    static uint8_t ReadLoBits(uint8_t byte, uint8_t bitsToRead);
    static uint8_t ReadMidBits(uint8_t byte, uint8_t start, uint8_t bitsToRead);
    
    inline static uint32_t EncodeZigZag32(int32_t n) {
        return (n << 1) ^ (n >> 31);
    }
    
    inline static uint64_t EncodeZigZag64(int64_t n) {
        return (n << 1) ^ (n >> 63);
    }

    inline static int32_t DecodeZigZag32(int32_t n) {
        return (n >> 1) ^ -(n & 1);
    }

    inline static int64_t DecodeZigZag64(int64_t n) {
        return (n >> 1) ^ -(n & 1);
    }
#ifdef _MSC_VER
    uint32_t static __inline ctz32(uint32_t value)
    {
        unsigned long trailing_zero = 0;

        if (_BitScanForward(&trailing_zero, value))
        {
            return trailing_zero;
        }
        else
        {
            // This is undefined, I better choose 32 than 0
            return 32;
        }
    }

    uint32_t static __inline clz32(uint32_t value)
    {
        unsigned long leading_zero = 0;

        if (_BitScanReverse(&leading_zero, value))
        {
            return 31 - leading_zero;
        }
        else
        {
            // Same remarks as above
            return 32;
        }
    }
    uint32_t static __inline ctz64(uint64_t value)
    {
        unsigned long trailing_zero = 0;

        if (_BitScanForward64(&trailing_zero, value))
        {
            return trailing_zero;
        }
        else
        {
            // This is undefined, I better choose 32 than 0
            return 64;
        }
    }

    uint32_t static __inline clz64(uint64_t value)
    {
        unsigned long leading_zero = 0;

        if (_BitScanReverse64(&leading_zero, value))
        {
            return 63 - leading_zero;
        }
        else
        {
            // Same remarks as above
            return 64;
        }
    }
#endif

private:
    template <typename T>
    static void WriteBits(
        std::vector<uint8_t>& buffer,
        uint32_t bitPosision,
        T value,
        uint8_t bitsToStore);

    template <typename T>
    static T ReadBits(
        const std::vector<uint8_t>& buffer,
        uint32_t bitPosision,
        uint8_t bitsToRead);
};

}} ////end of incolun::clothodb

