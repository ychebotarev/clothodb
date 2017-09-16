#include <intrin.h>
#include "bit_utils.h"

namespace clothodb {
namespace core {

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

void bit_utils::write_bits32(
    std::vector<uint8_t>& buffer,
    uint32_t bit_posision,
    uint32_t value,
    uint8_t bits_to_store)
{
    write_bits(buffer, bit_posision, value, bits_to_store);
}

void bit_utils::write_bits64(
    std::vector<uint8_t>& buffer,
    uint32_t bit_posision,
    uint64_t value,
    uint8_t bits_to_store)
{
    write_bits(buffer, bit_posision, value, bits_to_store);
}

//top = (n >> (8-bits_to_read)) & ((1 << bits_to_read) - 1)
uint8_t bit_utils::read_hi_bits(uint8_t byte, uint8_t bits_to_read)
{
    uint8_t shift = 8 - bits_to_read;
    uint8_t value = (byte >> shift) & mask[bits_to_read];
    return value;
}

//bottom = n & ((1 << bits_to_read) - 1)
uint8_t bit_utils::read_lo_bits(uint8_t byte, uint8_t bits_to_read)
{
    uint8_t value = byte & mask[bits_to_read];
    return value;
}

uint8_t bit_utils::read_mid_bits(uint8_t byte, uint8_t start, uint8_t bits_to_read)
{
    return read_hi_bits(byte << start, bits_to_read);
}

template <typename T>
void bit_utils::write_bits(
    std::vector<uint8_t>& buffer,
    uint32_t bit_posision,
    T value,
    uint8_t bits_to_store)
{
    int position = bit_posision >> 3;
    uint8_t available_bits = (bit_posision & 0x7) ? (8 - (bit_posision & 0x7)) : 0;

    //check if everything fits into current byte
    if (bits_to_store <= available_bits) 
    {
        buffer[position] +=  (uint8_t)(value << (available_bits - bits_to_store));
        return;
    }

    //fill current byte
    if (available_bits > 0) 
    {
        auto valueToAdd = value >> (bits_to_store - available_bits);
        buffer[position++] += (uint8_t)valueToAdd;
        bits_to_store -= available_bits;
    }

    //add whole byte
    while (bits_to_store >= 8)
    {
        char singleChar = (value >> (bits_to_store - 8)) & 0xFF;
        buffer[position++]= singleChar;
        bits_to_store -= 8;
    }

    //add remainder
    if (bits_to_store != 0) 
    {
        char singleChar = (value & ((1 << bits_to_store) - 1)) << (8 - bits_to_store);
        buffer[position] = singleChar;
    }
}

uint32_t bit_utils::read_bit(
    const std::vector<uint8_t>& buffer,
    uint32_t bit_posision)
{
    int position = bit_posision >> 3;
    bit_posision &= 0x7;
    bit_posision = 7 - bit_posision;
    auto ch = buffer[position];
    return ((ch & bitPositionMask[bit_posision]) != 0);
}

uint32_t bit_utils::read_bits32(
    const std::vector<uint8_t>& buffer,
    uint32_t bit_posision,
    uint8_t bits_to_read)
{
    return read_bits<uint32_t>(buffer, bit_posision, bits_to_read);
}

uint64_t bit_utils::read_bits64(
    const std::vector<uint8_t>& buffer,
    uint32_t bit_posision,
    uint8_t bits_to_read)
{
    return read_bits<uint64_t>(buffer, bit_posision, bits_to_read);
}

template <typename T>
T bit_utils::read_bits(
    const std::vector<uint8_t>& buffer,
    uint32_t bit_posision,
    uint8_t bits_to_read)
{
    int position = bit_posision >> 3;
    uint8_t available_bits = (bit_posision & 0x7) ? (8 - (bit_posision & 0x7)) : 0;

    T value = 0;
    if (bits_to_read < available_bits) 
    {
        return read_mid_bits(buffer[position], 8 - available_bits, bits_to_read);
    }

    if (available_bits > 0)
    {
        value += read_lo_bits(buffer[position++], available_bits);
        bits_to_read -= available_bits;
    }

    while (bits_to_read >= 8)
    {
        value = value << 8;
        value += buffer[position++];
        bits_to_read -= 8;
    }

    if (bits_to_read > 0)
    {
        value = value << bits_to_read;
        value += read_hi_bits(buffer[position], bits_to_read);
    }

    return value;
}

}}//end of incolun::clothdb namespace