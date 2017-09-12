#include "BitStream.h"

#include "src/TimeSeries/BitUtils.h"
#include "src/TimeSeries/ErrorCodes.h"

namespace incolun{
namespace clothodb{

using Uint32OrHResult = Result<uint32_t, uint32_t>;
using Uint64OrHResult = Result<uint64_t, uint32_t>;

static constexpr uint32_t kDefaultBufferSize = 2 * 60 * 60;//1 hour with 1 sec resolution, 2 bytes epr datapoint

BitStream::BitStream():BitStream(kDefaultBufferSize)
{}

BitStream::BitStream(size_t capacity)
    : m_commitedBits(0)
{
    m_buffer.resize(capacity);
}

void BitStream::WriteBit(uint32_t bitPosition, uint32_t value)
{
    WriteBits32(bitPosition, value, 1);
}

void BitStream::WriteBits32(uint32_t bitPosition, uint32_t value, uint8_t bitsToStore)
{
    SetCapacityBits(bitPosition + bitsToStore);
    BitUtils::WriteBits32(m_buffer, bitPosition, value, bitsToStore);
}

void BitStream::WriteBits64(uint32_t bitPosition, uint64_t value, uint8_t bitsToStore)
{
    SetCapacityBits(bitPosition + bitsToStore);
    BitUtils::WriteBits64(m_buffer, bitPosition, value, bitsToStore);
}

void BitStream::ReadCheck(uint32_t bitPosition) const
{
    if (bitPosition >= GetCommitedBits())
    {
        throw std::overflow_error("ReadBit");
    }
}

uint32_t BitStream::ReadBit(uint32_t bitPosition) const
{
    ReadCheck(bitPosition);
    auto result = BitUtils::ReadBit(m_buffer, bitPosition);
    return result;
}
uint32_t BitStream::ReadBits32(uint32_t bitPosition, uint8_t bitsToRead) const
{
    ReadCheck(bitPosition);
    auto result = BitUtils::ReadBits32(m_buffer, bitPosition, bitsToRead);
    return result;
}

uint64_t BitStream::ReadBits64(uint32_t bitPosition, uint8_t bitsToRead) const
{
    ReadCheck(bitPosition);
    auto result = BitUtils::ReadBits64(m_buffer, bitPosition, bitsToRead);
    return result;
}

inline void BitStream::SetCapacityBits(uint32_t capacity)
{
    while( (capacity >> 3) + 2 >= m_buffer.size())
    {
        m_buffer.resize(m_buffer.size() + m_buffer.size() / 2);
    }
}

uint32_t BitStreamReader::ReadBit()
{
    auto result = m_stream.ReadBit(m_bitPosition);
    ++m_bitPosition;
    return result;
}

uint32_t BitStreamReader::ReadBits32(uint8_t bitsToRead)
{
    auto result = m_stream.ReadBits32(m_bitPosition, bitsToRead);
    m_bitPosition += bitsToRead;
    return result;
}

uint64_t BitStreamReader::ReadBits64(uint8_t bitsToRead)
{
    auto result = m_stream.ReadBits64(m_bitPosition, bitsToRead);
    m_bitPosition += bitsToRead;
    return result;
}

void BitStreamWriter::WriteBit(uint32_t value)
{
    m_stream.WriteBit(m_bitPosition, value);
    ++m_bitPosition;
}
void BitStreamWriter::WriteBits32(uint32_t value, uint8_t bitsToStore)
{
    m_stream.WriteBits32(m_bitPosition, value, bitsToStore);
    m_bitPosition += bitsToStore;
}

void BitStreamWriter::WriteBits64(uint64_t value, uint8_t bitsToStore)
{
    m_stream.WriteBits64(m_bitPosition, value, bitsToStore);
    m_bitPosition += bitsToStore;
}

}} //end of namespace


