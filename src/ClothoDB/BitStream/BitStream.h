#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

namespace incolun{
namespace clothodb{

class BitStream
{
private:
    std::vector<uint8_t> buffer_;
    uint32_t bitsPosition_;
    uint32_t bitsLength_;

public:
    BitStream();
    BitStream(size_t capacity);

    void WriteBit(uint32_t value);
    void WriteBits32(uint32_t value, uint8_t bitsToStore);
    void WriteBits64(uint64_t value, uint8_t bitsToStore);

    uint32_t ReadBit();
    uint32_t ReadBits32(uint8_t bitsToRead);
    uint64_t ReadBits64(uint8_t bitsToRead);

    uint32_t ReadBits32(uint32_t bitsPosition, uint8_t bitsToRead)
    { 
        SetPosition(bitsPosition);
        return ReadBits32(bitsToRead);
    };
    
    uint64_t ReadBits64(uint32_t bitsPosition, uint8_t bitsToRead)
    {
        SetPosition(bitsPosition);
        return ReadBits64(bitsToRead);
    };

    uint8_t& operator[](std::size_t idx)
    { 
        return buffer_[idx]; 
    }

    void SetPosition(uint32_t bitsPosition) { bitsPosition_ = bitsPosition; }
    uint32_t GetPosition() const { return bitsPosition_; }

    void SetLength(uint32_t bitsLength) { bitsLength_ = bitsLength; }
	uint32_t GetLength() const { return bitsLength_; }
	
	void FixPosition() { bitsLength_ = bitsPosition_; }
	void Seal() { FixPosition(); }

    uint32_t CanRead() const { return bitsPosition_ < bitsLength_; }

private:
    inline void SetCapacityBits(uint32_t capacity);

};

}}