#include <limits>

#include "Compressor.h"

#include "src/TimeSeries/Constants.h"
#include "src/TimeSeries/BitUtils.h"
#include "src/TimeSeries/BitStream.h"

namespace incolun {
namespace clothodb {

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

TimestampCompressor::TimestampCompressor(BitStreamWriter& writer)
    :m_writer(writer)
{
}

void TimestampCompressor::AppendFirstValue(uint32_t timestamp)
{
    m_writer.WriteBits32(timestamp, Constants::kFirstTimestampBits);
    m_prevTimestamp = timestamp;
    m_prevTimestampDelta = 0;
}
void TimestampCompressor::AppendNextValue(uint32_t timestamp)
{
    int32_t delta = timestamp - m_prevTimestamp;

    StoreDeltaOfDelta(delta - m_prevTimestampDelta);

    m_prevTimestamp = timestamp;
    m_prevTimestampDelta = delta;
}

__inline void TimestampCompressor::StoreDeltaOfDelta(int32_t deltaOfDelta)
{
	if (deltaOfDelta == 0)
	{
        m_writer.WriteBit(0);
		return;
	}

	deltaOfDelta = BitUtils::EncodeZigZag32(deltaOfDelta);
	--deltaOfDelta;

	if (deltaOfDelta < 0x80)//7 bit
	{
		// '10' followed by a DofD if DofD is in range [0,127]
        m_writer.WriteBits32(deltaOfDelta | DELTAD_7_MASK, 9);
	}
	else if (deltaOfDelta <= 0x200) //9 bits
	{
		// '110' followed by a by a DofD if DofD is in range [128, 511]
        m_writer.WriteBits32(deltaOfDelta | DELTAD_9_MASK, 12);
	}
	else
	{
		// '111' followed by a value, 13 bits.
        m_writer.WriteBits32(deltaOfDelta | DELTAD_13_MASK, 16);
	}
}

DoubleCompressor::DoubleCompressor(BitStreamWriter& writer)
    :m_writer(writer)
{
    m_prevValue = 0;
    m_prevValueTZ = 0;
    m_prevValueLZ = 0;
}

void DoubleCompressor::AppendFirstValue(uint64_t* input)
{
    AppendNextValue(input);
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
void DoubleCompressor::AppendNextValue(uint64_t* input)
{
    uint64_t value = *input;
    uint64_t xorValue = m_prevValue ^ value;

    if (xorValue == 0)
    {
        m_writer.WriteBit(0);
        return;
    }

    m_writer.WriteBit(1);

    auto leadingZeros = BitUtils::clz64(xorValue);
    auto trailingZeros = BitUtils::ctz64(xorValue);

    if (leadingZeros > Constants::kDoubleMaxLeadingZerosLength)
    {
        leadingZeros = Constants::kDoubleMaxLeadingZerosLength;
    }

    auto blockBits = 64 - leadingZeros - trailingZeros;
    auto prevBlockBits = 64 - m_prevValueTZ - m_prevValueLZ;

    if (leadingZeros >= m_prevValueLZ
        && trailingZeros >= m_prevValueTZ
        && prevBlockBits < Constants::kDoubleBlockSizeLengthBits + blockBits)
    {
        m_writer.WriteBit(1);

        uint64_t blockValue = xorValue >> m_prevValueTZ;
        m_writer.WriteBits64(blockValue, prevBlockBits);
    }
    else
    {
        m_writer.WriteBit(0);
        m_writer.WriteBits32(leadingZeros, Constants::kDoubleLeadingZerosLengthBits);

        m_writer.WriteBits32(blockBits - 1, Constants::kDoubleBlockSizeLengthBits);

        uint64_t blockValue = xorValue >> trailingZeros;
        m_writer.WriteBits64(blockValue, blockBits);

        m_prevValueTZ = trailingZeros;
        m_prevValueLZ = leadingZeros;
    }
    m_prevValue = value;
}

IntegerCompressor::IntegerCompressor(BitStreamWriter& writer)
    :m_writer(writer)
{
}

void IntegerCompressor::AppendFirstValue(uint64_t* input)
{
    uint64_t value = *input;
    m_writer.WriteBits64(value, 64);
    m_prevValue = value;
    m_prevValueDelta = 0;
}

void IntegerCompressor::AppendNextValue(uint64_t* input)
{
    uint64_t value = *input;
    int64_t delta = value - m_prevValue;

    StoreDeltaOfDelta(delta - m_prevValueDelta);

    m_prevValue = value;
    m_prevValueDelta = delta;
}

__inline void IntegerCompressor::StoreDeltaOfDelta(int64_t deltaOfDeltaInput)
{
    if (deltaOfDeltaInput == 0)
    {
        m_writer.WriteBit(0);
        return;
    }

    uint64_t deltaOfDelta = BitUtils::EncodeZigZag64(deltaOfDeltaInput);
    --deltaOfDelta;

    if (deltaOfDelta < 0x80)//7 bit
    {
        // '10' followed by a DofD if DofD is in range [0,127]
        m_writer.WriteBits64(deltaOfDelta | DELTAD_7_MASK, 9);
    }
    else if (deltaOfDelta <= 0x200) //9 bits
    {
        // '110' followed by a by a DofD if DofD is in range [128, 511]
        m_writer.WriteBits64(deltaOfDelta | DELTAD_9_MASK, 12);
    }
    else if (deltaOfDelta <= 0xFFFF) //16 bits
    {
        // '1110' followed by a value, 16 bits.
        m_writer.WriteBits64(deltaOfDelta | DELTAD_16_MASK, 20);
    }
    else
    {
        m_writer.WriteBits32(0b1111,4);
        m_writer.WriteBits64(deltaOfDelta, 64);
    }
}

std::shared_ptr<BitStream> Compressor::CompressTimestamps(const std::vector<uint32_t>& values)
{
	std::shared_ptr<BitStream> stream(new BitStream((values.size() / 2)+2));
    if (values.size() == 0) return stream;

    BitStreamWriter writer(*stream.get());
    TimestampCompressor compressor(writer);

    compressor.AppendFirstValue(values[0]);
    for (size_t i = 1; i < values.size(); ++i)
    {
        compressor.AppendNextValue(values[i]);
    }
    writer.Commit();
	return stream;
}

std::shared_ptr<BitStream> Compressor::CompressDoubleValues(const std::vector<double>& values)
{
    std::shared_ptr<BitStream> stream(new BitStream(values.size() * 2));
    if (values.size() == 0) return stream;

    BitStreamWriter writer(*stream.get());
    DoubleCompressor compressor(writer);

    compressor.AppendFirstValue((uint64_t*)&values[0]);
    for (size_t i = 1; i < values.size(); ++i)
    {
        compressor.AppendNextValue((uint64_t*)&values[i]);
    }
    writer.Commit();
    return stream;
}

std::shared_ptr<BitStream> Compressor::CompressIntegerValues(const std::vector<uint64_t>& values)
{
    std::shared_ptr<BitStream> stream(new BitStream(values.size() * 2));
    if (values.size() == 0) return stream;

    BitStreamWriter writer(*stream.get());
    IntegerCompressor compressor(writer);

    compressor.AppendFirstValue((uint64_t*)&values[0]);
    for (size_t i = 1; i < values.size(); ++i)
    {
        compressor.AppendNextValue((uint64_t*)&values[i]);
    }
    writer.Commit();
    return stream;
}

}}