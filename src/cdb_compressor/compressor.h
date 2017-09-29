#pragma once

#include "src/cdb_compressor/bit_stream.h"

namespace cdb{
namespace compressor{

class compressor
{
public:
	static std::shared_ptr<bit_stream> compress_timestamps(const std::vector<uint32_t>& timestamps);
    static std::shared_ptr<bit_stream> compress_double_values(const std::vector<double>& values);
    static std::shared_ptr<bit_stream> compress_integer_values(const std::vector<uint64_t>& values);
};

class value_compressor
{
public:
    virtual void append_first_value(uint64_t value) = 0;
    virtual void append_next_value(uint64_t value) = 0;
};

//Compress timestamps
//Data should be stored in 2 hours chunk with 1 sec resolution
//Timestamp is aligned to the chunk start
//As the results all values are in range [0, 3600]
//also all values are expected to be increasing
class timestamp_compressor
{
public:
    timestamp_compressor(bit_stream_writer& writer);

    void append_first_value(uint32_t timestamp);
    void append_next_value(uint32_t timestamp);

    __forceinline void store_delta_of_delta(int32_t delta_of_delta);

private:
    bit_stream_writer& m_writer;
    uint32_t m_prev_timestamp;
    int32_t m_prev_timestamp_delta;
};

class double_compressor : public value_compressor
{
public:
    double_compressor(bit_stream_writer& writer);

    void append_first_value(uint64_t value);
    void append_next_value(uint64_t value);

private:
    bit_stream_writer& m_writer;
    uint64_t m_prev_value;
    uint32_t m_prev_value_TZ;
    uint32_t m_prev_value_LZ;
};

class integer_compressor : public value_compressor
{
public:
    integer_compressor(bit_stream_writer& writer);
    
    void append_first_value(uint64_t value);
    void append_next_value(uint64_t value);

    __forceinline void store_delta_of_delta(int64_t deltaOfDelta);

private:
    bit_stream_writer& m_writer;
    uint64_t m_prev_value;
    int64_t m_prev_value_delta;
};

}}