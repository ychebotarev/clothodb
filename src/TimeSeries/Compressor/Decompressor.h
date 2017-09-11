#pragma once

#include "src/timeseries/common.h"

namespace incolun{
namespace clothodb{

class BitStream;

class Decompressor
{
public:
    static VectorOrError<uint32_t> DecompressTimeStamps(BitStream& stream);
    static VectorOrError<double> DecompressDoubleValues(BitStream& stream);
    static VectorOrError<uint64_t> DecompressIntegerValues(BitStream& stream);

private:
    template<typename T, class D>
    static VectorOrError<T> Decompress(BitStream& stream);

};

class ValueDecompressor
{
public:
    virtual uint64_t GetFirstValue() = 0;
    virtual uint64_t GetNextValue() = 0;
};

class TimeStampDecompressor
{
public:
    TimeStampDecompressor(BitStream& stream);

    uint32_t GetFirstValue();
    uint32_t GetNextValue();

private:
    __inline int32_t ReadTimestampDeltaOfDelta();

    BitStream& m_stream;
    int32_t m_prevTimestampDelta;
    uint32_t m_prevTimestamp;
};

class DoubleDecompressor : public ValueDecompressor
{
public:
    DoubleDecompressor(BitStream& stream);
    
    virtual uint64_t GetFirstValue();
    virtual uint64_t GetNextValue();

private:

    BitStream& m_stream;
    uint64_t m_prevValue = 0;
    uint32_t m_prevValueTZ = 0;
    uint32_t m_prevValueLZ = 0;
};

class IntegerDecompressor : public ValueDecompressor
{
public:
    IntegerDecompressor(BitStream& stream);
    
    virtual uint64_t GetFirstValue();
    virtual uint64_t GetNextValue();

private:
    __inline int64_t ReadIntegerDeltaOfDelta();

    BitStream& m_stream;
    int64_t m_prevIntegerDelta;
    uint64_t m_prevInteger;
};

}}