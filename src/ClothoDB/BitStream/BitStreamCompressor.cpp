#include "BitStreamCompressor.h"

#include "BitStreamConstants.h"
#include "src/clothodb/BitUtils.h"
#include "src/clothodb/timeSeriesPoint.h"
#include "BitStream.h"

namespace incolun {
namespace clothodb {

static constexpr int DELTAD_7_MASK = 0x02 << 7;
static constexpr int DELTAD_9_MASK = 0x06 << 9;
static constexpr int DELTAD_13_MASK = 0x07 << 13;

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


BitStreamCompressor::BitStreamCompressor(std::shared_ptr<BitStream> stream)
{
    stream_ = stream;
}

BitStreamCompressor::~BitStreamCompressor()
{
}

// Store a delta of delta (DofD) using the following rules:
// '0' = DofD did not change
// '10' followed by a DofD if DofD is in range [0,127]
// '110' followed by a by a DofD if DofD is in range [128, 511]
// '111' followed by a value, 13 bits.
void BitStreamCompressor::AppendTimestamp(int32_t timestamp) 
{
    if (stream_->GetPosition() == 0) 
    {
        stream_->WriteBits32(timestamp, BitStreamConstants::kFirstTimestampBits);
        prevTimestamp_ = timestamp;
        prevTimestampDelta_ = BitStreamConstants::kDefaultDelta;
        return;
    }

    int32_t delta = timestamp - prevTimestamp_;
    int32_t deltaD = delta - prevTimestampDelta_;

    if (deltaD == 0) 
    {
        prevTimestamp_ = timestamp;
        stream_->WriteBit(0);
        return;
    }

    deltaD = BitUtils::EncodeZigZag32(deltaD);
    --deltaD;
    prevTimestamp_ = timestamp;
    prevTimestampDelta_ = delta;

    if (deltaD < 0x80)//7 bit
    {
        // '10' followed by a DofD if DofD is in range [0,127]
        stream_->WriteBits32(deltaD | DELTAD_7_MASK, 9);
        return;
    }

    if (deltaD <= 0x200) //9 bits
    {
        // '110' followed by a by a DofD if DofD is in range [128, 511]
        stream_->WriteBits32(deltaD | DELTAD_9_MASK, 12);
        return;
    }

    // '111' followed by a value, 13 bits.
    stream_->WriteBits32(deltaD | DELTAD_13_MASK, 16);
    return;
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

void BitStreamCompressor::AppendValue(int64_t value)
{
    uint64_t xorValue = prevValue_ ^ value;

    if (xorValue == 0) 
    {
        stream_->WriteBit(0);
        return;
    }

    stream_->WriteBit(1);

    auto leadingZeros = BitUtils::clz64(xorValue);
    auto trailingZeros = BitUtils::ctz64(xorValue);

    if (leadingZeros > BitStreamConstants::kMaxLeadingZerosLength) 
    {
        leadingZeros = BitStreamConstants::kMaxLeadingZerosLength;
    }

    auto blockBits = 64 - leadingZeros - trailingZeros;
    auto prevBlockBits = 64 - prevValueTZ_ - prevValueLZ_;

    if (leadingZeros >= prevValueLZ_ 
        && trailingZeros >= prevValueTZ_ 
        && prevBlockBits < BitStreamConstants::kBlockSizeLengthBits + blockBits)
    {
        stream_->WriteBits32(1, 1);

        uint64_t blockValue = xorValue >> prevValueTZ_;
        stream_->WriteBits64(blockValue, prevBlockBits);
    }
    else
    {
        stream_->WriteBit(0);
        stream_->WriteBits64(leadingZeros, BitStreamConstants::kLeadingZerosLengthBits);

        stream_->WriteBits64(blockBits - 1, BitStreamConstants::kBlockSizeLengthBits);

        uint64_t blockValue = xorValue >> trailingZeros;
        stream_->WriteBits64(blockValue, blockBits);

        prevValueTZ_ = trailingZeros;
        prevValueLZ_ = leadingZeros;
    }
    prevValue_ = value;
}

}}