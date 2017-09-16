#include <limits>

#include "compressor.h"

#include "src/core/constants.h"
#include "src/core/bit_utils.h"
#include "src/core/bit_stream.h"

namespace clothodb {
namespace core {

static constexpr int DELTAD_7_MASK = 0x02 << 7;
static constexpr int DELTAD_9_MASK = 0x06 << 9;
static constexpr int DELTAD_13_MASK = 0x07 << 13;
static constexpr int DELTAD_16_MASK = 0x0E << 16;

//using facebook gorilla paper

//For subsequent time stamps, tn:
//(a)Calculate the delta of delta :
//D = (t(n) - t(n-1)) - (t(n-1) - t(n-2))
//(b) If D is zero, then store a single '0' bit
//(c) If D is between[-63, 64], store '10' followed by
//the value(7 bits)
//(d) If D is between[-255, 256], store '110' followed by
//the value(9 bits)
//(e) if D is between[-2047, 2048], store '1110' followed
//by the value(12 bits)
//(f) Otherwise store '1111' followed by D using 32 bits

// According to gorillas paper one chunk contains 2 hours of data, with time resolution 1 sec
// this means that maximum numeber of data points stored in chanck in 2*60*60 = 7200, or 1 1100 0010 0000 in binary
// also this is maximum delta
// it means that maximun mumber of bits to store is 13
// so it doesn't make sense to store 32 bits ever.
// thus last rule is changed to 

//(e) Otherwise store ‘111’ followed by the value (13 bits)

timestamp_compressor::timestamp_compressor(bit_stream_writer& writer)
    :m_writer(writer)
{
}

void timestamp_compressor::append_first_value(uint32_t timestamp)
{
    m_writer.write_bits32(timestamp, Constants::kFirstTimestampBits);
    m_prev_timestamp = timestamp;
    m_prev_timestamp_delta = 0;
}
void timestamp_compressor::append_next_value(uint32_t timestamp)
{
    int32_t delta = timestamp - m_prev_timestamp;

    store_delta_of_delta(delta - m_prev_timestamp_delta);

    m_prev_timestamp = timestamp;
    m_prev_timestamp_delta = delta;
}

__inline void timestamp_compressor::store_delta_of_delta(int32_t delta_of_delta)
{
	if (delta_of_delta == 0)
	{
        m_writer.write_bit(0);
		return;
	}

    delta_of_delta = bit_utils::encode_zig_zag32(delta_of_delta);
	--delta_of_delta;

	if (delta_of_delta < 0x80)//7 bit
	{
		// '10' followed by a DofD if DofD is in range [0,127]
        m_writer.write_bits32(delta_of_delta | DELTAD_7_MASK, 9);
	}
	else if (delta_of_delta <= 0x200) //9 bits
	{
		// '110' followed by a by a DofD if DofD is in range [128, 511]
        m_writer.write_bits32(delta_of_delta | DELTAD_9_MASK, 12);
	}
	else
	{
		// '111' followed by a value, 13 bits.
        m_writer.write_bits32(delta_of_delta | DELTAD_13_MASK, 16);
	}
}

double_compressor::double_compressor(bit_stream_writer& writer)
    :m_writer(writer)
{
    m_prev_value = 0;
    m_prev_value_TZ = 0;
    m_prev_value_LZ = 0;
}

void double_compressor::append_first_value(uint64_t* input)
{
    append_next_value(input);
}

// Values are encoded by XORing them with the previous value.  
// If XOR result is zero value (value is the same as the previous
// value), only a single zero bit is stored.
// Otherwise 1 bit is stored. 

// For non-zero XORred results, there are two choices:
//
// 1) If the block of meaningful bits falls in between the block of
//    previous meaningful bits, i.e., there are at least as many
//    leading zeros and as many trailing zeros as with the previous
//    value, use that information for the block position and just
//    store the XORred value.
//
// 2) Length of the number of leading zeros is stored in the next 5
//    bits, then length of the XORred value is stored in the next 6
//    bits and finally the XORred value is stored.
void double_compressor::append_next_value(uint64_t* input)
{
    uint64_t value = *input;
    uint64_t xor_value = m_prev_value ^ value;

    if (xor_value == 0)
    {
        m_writer.write_bit(0);
        return;
    }

    m_writer.write_bit(1);

    auto leading_zeros = bit_utils::clz64(xor_value);
    auto trailing_zeros = bit_utils::ctz64(xor_value);

    if (leading_zeros > Constants::kDoubleMaxLeadingZerosLength)
    {
        leading_zeros = Constants::kDoubleMaxLeadingZerosLength;
    }

    auto block_bits = 64 - leading_zeros - trailing_zeros;
    auto prev_block_bits = 64 - m_prev_value_TZ - m_prev_value_LZ;

    if (leading_zeros >= m_prev_value_LZ
        && trailing_zeros >= m_prev_value_TZ
        && prev_block_bits < Constants::kDoubleBlockSizeLengthBits + block_bits)
    {
        m_writer.write_bit(1);

        uint64_t block_value = xor_value >> m_prev_value_TZ;
        m_writer.write_bits64(block_value, prev_block_bits);
    }
    else
    {
        m_writer.write_bit(0);
        m_writer.write_bits32(leading_zeros, Constants::kDoubleLeadingZerosLengthBits);

        m_writer.write_bits32(block_bits - 1, Constants::kDoubleBlockSizeLengthBits);

        uint64_t block_value = xor_value >> trailing_zeros;
        m_writer.write_bits64(block_value, block_bits);

        m_prev_value_TZ = trailing_zeros;
        m_prev_value_LZ = leading_zeros;
    }
    m_prev_value = value;
}

integer_compressor::integer_compressor(bit_stream_writer& writer)
    :m_writer(writer)
{
}

void integer_compressor::append_first_value(uint64_t* input)
{
    uint64_t value = *input;
    m_writer.write_bits64(value, 64);
    m_prev_value = value;
    m_prev_value_delta = 0;
}

void integer_compressor::append_next_value(uint64_t* input)
{
    uint64_t value = *input;
    int64_t delta = value - m_prev_value;

    store_delta_of_delta(delta - m_prev_value_delta);

    m_prev_value = value;
    m_prev_value_delta = delta;
}

__inline void integer_compressor::store_delta_of_delta(int64_t delta_of_delta_input)
{
    if (delta_of_delta_input == 0)
    {
        m_writer.write_bit(0);
        return;
    }

    uint64_t delta_of_delta = bit_utils::encode_zig_zag64(delta_of_delta_input);
    --delta_of_delta;

    if (delta_of_delta < 0x80)//7 bit
    {
        // '10' followed by a DofD if DofD is in range [0,127]
        m_writer.write_bits64(delta_of_delta | DELTAD_7_MASK, 9);
    }
    else if (delta_of_delta <= 0x200) //9 bits
    {
        // '110' followed by a by a DofD if DofD is in range [128, 511]
        m_writer.write_bits64(delta_of_delta | DELTAD_9_MASK, 12);
    }
    else if (delta_of_delta <= 0xFFFF) //16 bits
    {
        // '1110' followed by a value, 16 bits.
        m_writer.write_bits64(delta_of_delta | DELTAD_16_MASK, 20);
    }
    else
    {
        m_writer.write_bits32(0b1111,4);
        m_writer.write_bits64(delta_of_delta, 64);
    }
}

std::shared_ptr<bit_stream> compressor::compress_timestamps(const std::vector<uint32_t>& values)
{
	std::shared_ptr<bit_stream> stream(new bit_stream((values.size() / 2)+2));
    if (values.size() == 0) return stream;

    bit_stream_writer writer(*stream.get());
    timestamp_compressor compressor(writer);

    compressor.append_first_value(values[0]);
    for (size_t i = 1; i < values.size(); ++i)
    {
        compressor.append_next_value(values[i]);
    }
    writer.commit();
	return stream;
}

std::shared_ptr<bit_stream> compressor::compress_double_values(const std::vector<double>& values)
{
    std::shared_ptr<bit_stream> stream(new bit_stream(values.size() * 2));
    if (values.size() == 0) return stream;

    bit_stream_writer writer(*stream.get());
    double_compressor compressor(writer);

    compressor.append_first_value((uint64_t*)&values[0]);
    for (size_t i = 1; i < values.size(); ++i)
    {
        compressor.append_next_value((uint64_t*)&values[i]);
    }
    writer.commit();
    return stream;
}

std::shared_ptr<bit_stream> compressor::compress_integer_values(const std::vector<uint64_t>& values)
{
    std::shared_ptr<bit_stream> stream(new bit_stream(values.size() * 2));
    if (values.size() == 0) return stream;

    bit_stream_writer writer(*stream.get());
    integer_compressor compressor(writer);

    compressor.append_first_value((uint64_t*)&values[0]);
    for (size_t i = 1; i < values.size(); ++i)
    {
        compressor.append_next_value((uint64_t*)&values[i]);
    }
    writer.commit();
    return stream;
}

}}