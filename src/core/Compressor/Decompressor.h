#pragma once

#include "src/core/common.h"
#include "src/core/bit_stream.h"

namespace clothodb {
namespace core {

class decompressor
{
public:
    static vector_or_error<uint32_t> decompress_timestamps(bit_stream& stream);
    static vector_or_error<double> decompress_double_values(bit_stream& stream);
    static vector_or_error<uint64_t> decompress_integer_values(bit_stream& stream);

private:
    template<typename T, class D>
    static vector_or_error<T> decompress(bit_stream& stream);

};

class value_decompressor
{
public:
    virtual uint64_t get_first_value() = 0;
    virtual uint64_t get_next_value() = 0;
};

class timestamp_decompressor
{
public:
    timestamp_decompressor(bit_stream_reader& reader);

    uint32_t get_first_value();
    uint32_t get_next_value();

private:
    __inline int32_t read_delta_of_delta();

    bit_stream_reader& m_reader;
    int32_t m_prev_timestamp_delta;
    uint32_t m_prev_timestamp;
};

class double_decompressor : public value_decompressor
{
public:
    double_decompressor(bit_stream_reader& reader);
    
    virtual uint64_t get_first_value();
    virtual uint64_t get_next_value();

private:

    bit_stream_reader& m_reader;
    uint64_t m_prev_value = 0;
    uint32_t m_prev_value_TZ = 0;
    uint32_t m_prev_value_LZ = 0;
};

class integer_decompressor : public value_decompressor
{
public:
    integer_decompressor(bit_stream_reader& reader);
    
    virtual uint64_t get_first_value();
    virtual uint64_t get_next_value();

private:
    __inline int64_t read_delta_of_delta();

    bit_stream_reader& m_reader;
    int64_t m_prev_integer_delta;
    uint64_t m_prev_integer;
};

}}