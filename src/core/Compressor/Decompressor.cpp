#include "src/core/compressor/Decompressor.h"

#include "src/core/Constants.h"
#include "src/core/bit_utils.h"
#include "src/core/ErrorCodes.h"

namespace clothodb {
namespace core {

timestamp_decompressor::timestamp_decompressor(bit_stream_reader& reader)
    :m_reader(reader)
{
    m_prev_timestamp_delta = 0;
    m_prev_timestamp = 0;
}

uint32_t timestamp_decompressor::get_first_value()
{
    m_prev_timestamp = m_reader.read_bits32(Constants::kFirstTimestampBits);
    m_prev_timestamp_delta = 0;
    return m_prev_timestamp;
}

uint32_t timestamp_decompressor::get_next_value()
{
    auto delta_of_delta = read_delta_of_delta();

    m_prev_timestamp_delta += delta_of_delta;
    m_prev_timestamp += m_prev_timestamp_delta;
    return m_prev_timestamp;
}

__inline int32_t timestamp_decompressor::read_delta_of_delta()
{
    if (m_reader.read_bit() == 0)
    {
        return 0;
    }
    
    uint32_t delta_of_delta = 0;
    if (m_reader.read_bit() == 0)
    {
        delta_of_delta = m_reader.read_bits32(7);
    }
    else if (m_reader.read_bit() == 0)
    {
        delta_of_delta = m_reader.read_bits32(9);
    }
    else
    {
        delta_of_delta = m_reader.read_bits32(13);
    }
    return bit_utils::decode_zig_zag32(++delta_of_delta);
}
        
double_decompressor::double_decompressor(bit_stream_reader& reader)
    :m_reader(reader)
{
    m_prev_value = 0;
    m_prev_value_TZ = 0;
    m_prev_value_LZ = 0;
}

uint64_t double_decompressor::get_first_value()
{
    return get_next_value();
}

uint64_t double_decompressor::get_next_value()
{
    uint64_t xorValue = 0;
    if (m_reader.read_bit() == 0)
    {
        m_prev_value = m_prev_value ^ xorValue;
    }
    else if (m_reader.read_bit() == 0)
    {
        m_prev_value_LZ = m_reader.read_bits32(Constants::kDoubleLeadingZerosLengthBits);
        uint32_t blockSize = m_reader.read_bits32(Constants::kDoubleBlockSizeLengthBits) + 1;
        m_prev_value_TZ = 64 - blockSize - m_prev_value_LZ;

        xorValue = m_reader.read_bits64(blockSize);
        xorValue <<= m_prev_value_TZ;
    }
    else
    {
        //use information from previous block
        xorValue = m_reader.read_bits64(64 - m_prev_value_TZ - m_prev_value_LZ);
        xorValue <<= m_prev_value_TZ;
    }

    uint64_t value = xorValue ^ m_prev_value;
    m_prev_value = value;
    return value;
}

integer_decompressor::integer_decompressor(bit_stream_reader& reader)
    :m_reader(reader)
{
    m_prev_integer_delta = 0;
    m_prev_integer = 0;
}

uint64_t integer_decompressor::get_first_value()
{
    m_prev_integer = m_reader.read_bits64(64);
    m_prev_integer_delta = 0;
    return m_prev_integer;
}

uint64_t integer_decompressor::get_next_value()
{
    auto delta_of_delta = read_delta_of_delta();

    m_prev_integer_delta += delta_of_delta;
    m_prev_integer += m_prev_integer_delta;
    return m_prev_integer;
}

__inline int64_t integer_decompressor::read_delta_of_delta()
{
    if (m_reader.read_bit() == 0)
    {
        return 0;
    }

    uint64_t delta_of_delta = 0;
    if (m_reader.read_bit() == 0)
    {
        delta_of_delta = m_reader.read_bits32(7);
    }
    else if (m_reader.read_bit() == 0)
    {
        delta_of_delta = m_reader.read_bits32(9);
    }
    else if (m_reader.read_bit() == 0)
    {
        delta_of_delta = m_reader.read_bits32(16);
    }
    else
    {
        delta_of_delta = m_reader.read_bits64(64);
    }
    return bit_utils::decode_zig_zag64(++delta_of_delta);
}

vector_or_error<uint32_t> decompressor::decompress_timestamps(bit_stream& stream)
{
    return decompress<uint32_t, timestamp_decompressor>(stream);
}

vector_or_error<uint64_t> decompressor::decompress_integer_values(bit_stream& stream)
{
    return decompress<uint64_t, integer_decompressor>(stream);
}

vector_or_error<double> decompressor::decompress_double_values(bit_stream& stream)
{
    return decompress<double, double_decompressor>(stream);
}

template<typename T, class D>
vector_or_error<T> decompressor::decompress(bit_stream& stream)
{
    bit_stream_reader reader(stream);
    
    std::shared_ptr<std::vector<T>> result = std::make_shared<std::vector<T>>();
    if (stream.is_empty()) return vector_or_error<T>::from_value(result);

    try
    {
        D decompressor(reader);
        if (reader.can_read())
        {
            auto first_value = decompressor.get_first_value();
            result->push_back(*((T*)&first_value));
        }

        while (reader.can_read())
        {
            auto next_value = decompressor.get_next_value();
            result->push_back(*((T*)&next_value));
        }

        return vector_or_error<T>::from_value(result);
    }
    catch (const std::overflow_error&)
    {
        return vector_or_error<T>::from_error(ErrorCodes::kBitStreamOverflaw);
    }
}

}}