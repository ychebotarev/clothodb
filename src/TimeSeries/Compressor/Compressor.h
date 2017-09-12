#pragma once

#include "src/timeseries/common.h"
#include "src/timeseries/bitstream.h"

namespace incolun{
namespace clothodb{

class Compressor
{
public:
	static std::shared_ptr<BitStream> CompressTimestamps(const std::vector<uint32_t>& timestamps);
    static std::shared_ptr<BitStream> CompressDoubleValues(const std::vector<double>& values);
    static std::shared_ptr<BitStream> CompressIntegerValues(const std::vector<uint64_t>& values);
};

class ValueCompressor
{
public:
    virtual void AppendFirstValue(uint64_t* input) = 0;
    virtual void AppendNextValue(uint64_t* input) = 0;
};

//Compress timestamps
//Data should be stored in 2 hours chunk with 1 sec resolution
//Timestamp is aligned to the chunk start
//As the results all values are in range [0, 3600]
//also all values are expected to be increasing
class TimestampCompressor
{
public:
    TimestampCompressor(BitStreamWriter& writer);

    void AppendFirstValue(uint32_t timestamp);
    void AppendNextValue(uint32_t timestamp);

private:
    __inline void StoreDeltaOfDelta(int32_t deltaOfDelta);

private:
    BitStreamWriter& m_writer;
    uint32_t m_prevTimestamp;
    int32_t m_prevTimestampDelta;
};

class DoubleCompressor : public ValueCompressor
{
public:
    DoubleCompressor(BitStreamWriter& writer);

    void AppendFirstValue(uint64_t* input);
    void AppendNextValue(uint64_t* input);

private:
    BitStreamWriter& m_writer;
    uint64_t m_prevValue;
    uint32_t m_prevValueTZ;
    uint32_t m_prevValueLZ;
};

class IntegerCompressor : public ValueCompressor
{
public:
    IntegerCompressor(BitStreamWriter& writer);
    
    void AppendFirstValue(uint64_t* input);
    void AppendNextValue(uint64_t* input);

private:
    __inline void StoreDeltaOfDelta(int64_t deltaOfDelta);

private:
    BitStreamWriter& m_writer;
    uint64_t m_prevValue;
    int64_t m_prevValueDelta;
};

}}