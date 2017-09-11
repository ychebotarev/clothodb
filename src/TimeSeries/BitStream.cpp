#include "BitStream.h"

#include "src/TimeSeries/BitUtils.h"
#include "src/TimeSeries/ErrorCodes.h"

namespace incolun{
namespace clothodb{

using Uint32OrHResult = Result<uint32_t, uint32_t>;
using Uint64OrHResult = Result<uint64_t, uint32_t>;

static constexpr uint32_t kDefaultBufferSize = 2 * 60 * 60;//2 hours with 1 sec resolution

BitStream::BitStream():BitStream(kDefaultBufferSize)
{

}

BitStream::BitStream(size_t capacity)
{
    m_bitsPosition = 0;
    m_bitsLength = 0;
    m_buffer.resize(capacity);
}

void BitStream::WriteBit(uint32_t value) 
{
    SetCapacityBits(m_bitsPosition + 1);
    WriteBits32(value, 1);
}

void BitStream::WriteBits32(uint32_t value, uint8_t bitsToStore)
{
    SetCapacityBits(m_bitsPosition + bitsToStore);
    BitUtils::WriteBits32(m_buffer, m_bitsPosition, value, bitsToStore);
    m_bitsPosition += bitsToStore;
}

void BitStream::WriteBits64(uint64_t value, uint8_t bitsToStore)
{
    SetCapacityBits(m_bitsPosition + bitsToStore);
    BitUtils::WriteBits64(m_buffer, m_bitsPosition, value, bitsToStore);
    m_bitsPosition += bitsToStore;
}

uint32_t BitStream::ReadBit()
{
    if (!CanRead(1)) throw std::overflow_error("ReadBit");

    auto result = BitUtils::ReadBit(m_buffer, m_bitsPosition);
    ++m_bitsPosition;
	return result;
}

uint32_t BitStream::ReadBits32(uint8_t bitsToRead)
{
    if (!CanRead(bitsToRead)) throw std::overflow_error("ReadBits32");

    auto result = BitUtils::ReadBits32(m_buffer, m_bitsPosition, bitsToRead);
    m_bitsPosition += bitsToRead;
	return result;
}

uint64_t BitStream::ReadBits64(uint8_t bitsToRead)
{
    if (!CanRead(bitsToRead)) throw std::overflow_error("ReadBits64");

    auto result = BitUtils::ReadBits64(m_buffer, m_bitsPosition, bitsToRead);
    m_bitsPosition += bitsToRead;
	return result;
}

inline void BitStream::SetCapacityBits(uint32_t capacity)
{
    while( (capacity >> 3) + 2 >= m_buffer.size())
    {
        m_buffer.resize(m_buffer.size() + m_buffer.size() / 2);
    }
}

}} //end of namespace


