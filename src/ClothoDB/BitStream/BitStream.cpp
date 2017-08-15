#include "BitStream.h"

#include "src/ClothoDB/BitUtils.h"

namespace incolun{
namespace clothodb{

static constexpr uint32_t kDefaultBufferSize = 2 * 60 * 60;//2 hours with 1 sec resolution

BitStream::BitStream():BitStream(kDefaultBufferSize)
{

}

BitStream::BitStream(size_t capacity)
{
    bitsPosition_ = 0;
    bitsLength_ = 0;
    buffer_.resize(capacity);
}

void BitStream::WriteBit(uint32_t value) 
{
    SetCapacityBits(bitsPosition_ + 1);
    WriteBits32(value, 1);
}

void BitStream::WriteBits32(uint32_t value, uint8_t bitsToStore)
{
    SetCapacityBits(bitsPosition_ + bitsToStore);
    BitUtils::WriteBits32(buffer_, bitsPosition_, value, bitsToStore);
    bitsPosition_ += bitsToStore;
}

void BitStream::WriteBits64(uint64_t value, uint8_t bitsToStore)
{
    SetCapacityBits(bitsPosition_ + bitsToStore);
    BitUtils::WriteBits64(buffer_, bitsPosition_, value, bitsToStore);
    bitsPosition_ += bitsToStore;
}

uint32_t BitStream::ReadBit() 
{
	auto result = BitUtils::ReadBit(buffer_, bitsPosition_);
    auto result1 = BitUtils::ReadBits32(buffer_, bitsPosition_, 1);
    ++bitsPosition_;
	return result;
}

uint32_t BitStream::ReadBits32(uint8_t bitsToRead)
{
    auto result = BitUtils::ReadBits32(buffer_, bitsPosition_, bitsToRead);
	bitsPosition_ += bitsToRead;
	return result;
}

uint64_t BitStream::ReadBits64(uint8_t bitsToRead)
{
	auto result = BitUtils::ReadBits64(buffer_, bitsPosition_, bitsToRead);
	bitsPosition_ += bitsToRead;
	return result;
}

inline void BitStream::SetCapacityBits(uint32_t capacity)
{
    while( (capacity >> 3) >= buffer_.size())
    {
        buffer_.resize(buffer_.size() + buffer_.size() / 2);
    }
}


}} //end of namespace


