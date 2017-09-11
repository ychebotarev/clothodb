#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include "src/common/Result.h"

namespace incolun{
namespace clothodb{

using Uint32OrHResult = Result<uint32_t, uint32_t>;
using Uint64OrHResult = Result<uint64_t, uint32_t>;

class BitStream
{
private:
    std::vector<uint8_t> m_buffer;
    uint32_t m_bitsPosition;
    uint32_t m_bitsLength;

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
        return m_buffer[idx];
    }

    void SetPosition(uint32_t bitsPosition) { m_bitsPosition = bitsPosition; }
    uint32_t GetPosition() const { return m_bitsPosition; }
    bool IsEmpty() const { return m_bitsLength == 0; }
    void SetLength(uint32_t bitsLength) { m_bitsLength = bitsLength; }
	uint32_t GetLength() const { return m_bitsLength; }
	
    void Seal() { m_bitsLength = m_bitsPosition; SetPosition(0); }

    bool CanRead() const { return m_bitsPosition < m_bitsLength; }
    bool CanRead(uint32_t bitsToRead) const { return (m_bitsPosition+bitsToRead) <= m_bitsLength; }

private:
    inline void SetCapacityBits(uint32_t capacity);

};

}}