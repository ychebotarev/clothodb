#include "src/cdb_compressor/bit_stream.h"
#include "src/cdb_compressor/bit_utils.h"
#include "src/cdb_common/error_codes.h"

namespace cdb {
namespace compressor{

using namespace cdb;

static constexpr uint32_t kDefaultBufferSize = 2 * 60 * 60;//1 hour with 1 sec resolution, 2 bytes epr datapoint

bit_stream::bit_stream():bit_stream(kDefaultBufferSize)
{}

bit_stream::bit_stream(size_t capacity)
    : m_commited_bits(0)
{
    m_buffer.resize(capacity);
}

void bit_stream::write_bit(uint32_t bit_position, uint32_t value)
{
    write_bits32(bit_position, value, 1);
}

void bit_stream::write_bits32(uint32_t bit_position, uint32_t value, uint8_t bits_to_store)
{
    set_capacity_bits(bit_position + bits_to_store);
    bit_utils::write_bits32(m_buffer, bit_position, value, bits_to_store);
}

void bit_stream::write_bits64(uint32_t bit_position, uint64_t value, uint8_t bits_to_store)
{
    set_capacity_bits(bit_position + bits_to_store);
    bit_utils::write_bits64(m_buffer, bit_position, value, bits_to_store);
}

__forceinline void bit_stream::read_check(uint32_t bit_position) const
{
    if (bit_position >= get_commited_bits())
    {
        throw std::overflow_error("ReadBit");
    }
}

uint32_t bit_stream::read_bit(uint32_t bit_position) const
{
    read_check(bit_position);
    auto result = bit_utils::read_bit(m_buffer, bit_position);
    return result;
}
uint32_t bit_stream::read_bits32(uint32_t bit_position, uint8_t bitsToRead) const
{
    read_check(bit_position);
    auto result = bit_utils::read_bits32(m_buffer, bit_position, bitsToRead);
    return result;
}

uint64_t bit_stream::read_bits64(uint32_t bit_position, uint8_t bitsToRead) const
{
    read_check(bit_position);
    auto result = bit_utils::read_bits64(m_buffer, bit_position, bitsToRead);
    return result;
}

inline void bit_stream::set_capacity_bits(uint32_t capacity)
{
    while( (capacity >> 3) + 2 >= m_buffer.size())
    {
        m_buffer.resize(m_buffer.size() + m_buffer.size() / 2);
    }
}

uint32_t bit_stream_reader::read_bit()
{
    auto result = m_stream.read_bit(m_bit_position);
    ++m_bit_position;
    return result;
}

uint32_t bit_stream_reader::read_bits32(uint8_t bitsToRead)
{
    auto result = m_stream.read_bits32(m_bit_position, bitsToRead);
    m_bit_position += bitsToRead;
    return result;
}

uint64_t bit_stream_reader::read_bits64(uint8_t bitsToRead)
{
    auto result = m_stream.read_bits64(m_bit_position, bitsToRead);
    m_bit_position += bitsToRead;
    return result;
}

void bit_stream_writer::write_bit(uint32_t value)
{
    m_stream.write_bit(m_bit_position, value);
    ++m_bit_position;
}
void bit_stream_writer::write_bits32(uint32_t value, uint8_t bits_to_store)
{
    m_stream.write_bits32(m_bit_position, value, bits_to_store);
    m_bit_position += bits_to_store;
}

void bit_stream_writer::write_bits64(uint64_t value, uint8_t bits_to_store)
{
    m_stream.write_bits64(m_bit_position, value, bits_to_store);
    m_bit_position += bits_to_store;
}

}} //end of namespace