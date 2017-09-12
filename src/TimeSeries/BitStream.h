#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include "src/common/Result.h"

namespace incolun{
namespace clothodb{

using Uint32OrHResult = Result<uint32_t, uint32_t>;
using Uint64OrHResult = Result<uint64_t, uint32_t>;

//class is NOT thread safe
class BitStream
{
private:
    std::vector<uint8_t> m_buffer;
    uint32_t m_commitedBits;

public:
    BitStream();
    BitStream(size_t capacity);

    void WriteBit(uint32_t bitPosition, uint32_t value);
    void WriteBits32(uint32_t bitPosition, uint32_t value, uint8_t bitsToStore);
    void WriteBits64(uint32_t bitPosition, uint64_t value, uint8_t bitsToStore);

    uint32_t ReadBit(uint32_t bitPosition) const;
    uint32_t ReadBits32(uint32_t bitPosition, uint8_t bitsToRead) const;
    uint64_t ReadBits64(uint32_t bitPosition, uint8_t bitsToRead) const;

    uint8_t& operator[](std::size_t idx)
    { 
        return m_buffer[idx];
    }

    bool IsEmpty() const { return m_commitedBits == 0; }
    void Commit(uint32_t bitsLength) { m_commitedBits += bitsLength; }
	uint32_t GetCommitedBits() const { return m_commitedBits; }
    void SetCommitedBits(uint32_t bitsLength) { m_commitedBits = bitsLength; }

private:
    inline void SetCapacityBits(uint32_t capacity);
    __inline void ReadCheck(uint32_t bitPosition) const;
};

class BitStreamReader
{
public:
    BitStreamReader(const BitStream& stream) : m_stream(stream), m_bitPosition(0)
    {}
    uint32_t ReadBit();
    uint32_t ReadBits32(uint8_t bitToRead);
    uint64_t ReadBits64(uint8_t bitToRead);

    void SetPosition(uint32_t bitPosition) { m_bitPosition = bitPosition; }
    uint32_t GetPosition() const { return m_bitPosition; }
    bool CanRead() const { return m_bitPosition < m_stream.GetCommitedBits(); }
private:
    const BitStream& m_stream;
    uint32_t m_bitPosition;
};

class BitStreamWriter
{
public:
    BitStreamWriter(BitStream& stream) : m_stream(stream), m_bitPosition(0)
    {}

    void WriteBit(uint32_t value);
    void WriteBits32(uint32_t value, uint8_t bitsToStore);
    void WriteBits64(uint64_t value, uint8_t bitsToStore);

    void SetPosition(uint32_t bitsPosition) { m_bitPosition = bitsPosition; }
    uint32_t GetPosition() const { return m_bitPosition; }

    void Commit() { m_stream.SetCommitedBits(m_bitPosition); }
private:
    BitStream& m_stream;
    uint32_t m_bitPosition;
};

}}