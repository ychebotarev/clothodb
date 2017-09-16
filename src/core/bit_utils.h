#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <intrin.h>

namespace clothodb {
namespace core {

class bit_utils
{
public:
    // Write specified number of bits from value into byte buffer
    // bit_posision indicates starting position in bits
    static void write_bits32(
        std::vector<uint8_t>& buffer,
        uint32_t bit_posision,
        uint32_t value,
        uint8_t bits_to_store);

    static void write_bits64(
        std::vector<uint8_t>& buffer,
        uint32_t bit_posision,
        uint64_t value,
        uint8_t bits_to_store);

    static uint32_t read_bit(
        const std::vector<uint8_t>& buffer,
        uint32_t bit_posision);

    static uint32_t read_bits32(
        const std::vector<uint8_t>& buffer,
        uint32_t bit_posision,
        uint8_t bits_to_read);

    static uint64_t read_bits64(
        const std::vector<uint8_t>& buffer,
        uint32_t bit_posision,
        uint8_t bits_to_read);

    static uint8_t read_hi_bits(uint8_t byte, uint8_t bits_to_read);
    static uint8_t read_lo_bits(uint8_t byte, uint8_t bits_to_read);
    static uint8_t read_mid_bits(uint8_t byte, uint8_t start, uint8_t bits_to_read);
    
    inline static uint32_t encode_zig_zag32(int32_t n) {
        return (n << 1) ^ (n >> 31);
    }
    
    inline static uint64_t encode_zig_zag64(int64_t n) {
        return (n << 1) ^ (n >> 63);
    }

    inline static int32_t decode_zig_zag32(uint32_t n) {
        return (n >> 1) ^ -((int32_t)n & 1);
    }

    inline static int64_t decode_zig_zag64(uint64_t n) {
        return (n >> 1) ^ -((int64_t)n & 1);
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
    static void write_bits(
        std::vector<uint8_t>& buffer,
        uint32_t bit_posision,
        T value,
        uint8_t bits_to_store);

    template <typename T>
    static T read_bits(
        const std::vector<uint8_t>& buffer,
        uint32_t bit_posision,
        uint8_t bits_to_store);
};

}} ////end of incolun::clothodb

