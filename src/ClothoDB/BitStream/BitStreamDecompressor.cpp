#include "BitStreamDecompressor.h"

#include "BitStreamConstants.h"
#include "src/clothodb/BitUtils.h"
#include "src/clothodb/timeSeriesPoint.h"
#include "BitStream.h"

namespace incolun {
namespace clothodb {

__inline uint32_t ReadDeltaOfDelta(const std::shared_ptr<BitStream>& stream_, int prevDeltaOfDelta)
{
    int deltaOfDelta = prevDeltaOfDelta;
    if (stream_->ReadBit() == 0)
    {
        return deltaOfDelta;
    }
    
    if (stream_->ReadBit() == 0)
    {
        deltaOfDelta = stream_->ReadBits32(7);
    }
    else if (stream_->ReadBit() == 0)
    {
        deltaOfDelta = stream_->ReadBits32(9);
    }
    else
    {
        deltaOfDelta = stream_->ReadBits32(13);
    }
    return BitUtils::DecodeZigZag32(++deltaOfDelta);
}


std::vector<TimeSeriesPoint> BitStreamDecompressor::Decompress(const std::shared_ptr<BitStream>& stream)
{
    stream->SetPosition(0);
    uint32_t timestamp = stream->ReadBits32(BitStreamConstants::kFirstTimestampBits);
    uint32_t prevTimestampDelta = 0;
    uint32_t deltaOfDelta = 0;

    TimeSeriesPoint point;

    point.timestamp_ = timestamp;
    std::vector<TimeSeriesPoint> result;
    result.push_back(point);

    while (stream->CanRead())
    {
        deltaOfDelta = ReadDeltaOfDelta(stream, deltaOfDelta);

        prevTimestampDelta += deltaOfDelta;
        timestamp += prevTimestampDelta;
        point.timestamp_ = timestamp;
        result.push_back(point);
    }

    return result;
}

std::vector<uint32_t> BitStreamDecompressor::DecompressTimeStamps(const std::shared_ptr<BitStream>& stream)
{
    stream->SetPosition(0);
    uint32_t timestamp = stream->ReadBits32(BitStreamConstants::kFirstTimestampBits);
    uint32_t prevDelta = 0;
    uint32_t deltaOfDelta = 0;

    std::vector<uint32_t> result;
    result.push_back(timestamp);

    while (stream->CanRead())
    {
        deltaOfDelta = ReadDeltaOfDelta(stream, deltaOfDelta);

        prevDelta += deltaOfDelta;
        timestamp += prevDelta;
        result.push_back(timestamp);
    }

    return result;
}

std::vector<double> BitStreamDecompressor::DecompressValues(const std::shared_ptr<BitStream>& stream)
{
    uint64_t prevValue = 0;
    uint32_t prevValueTZ = 0;
    uint32_t prevValueLZ = 0;

    stream->SetPosition(0);
    std::vector<double> result;

    while (stream->CanRead())
    {
        uint64_t xorValue = 0;
        if (stream->ReadBit() == 0)
        {
            prevValue = prevValue ^ xorValue;
        }
        else if (stream->ReadBit() == 0)
        {
            prevValueLZ = stream->ReadBits32(BitStreamConstants::kLeadingZerosLengthBits);
            uint32_t blockSize = stream->ReadBits32(BitStreamConstants::kBlockSizeLengthBits) + 1;
            prevValueTZ = 64 - blockSize - prevValueLZ;

            xorValue = stream->ReadBits64(blockSize);
            xorValue <<= prevValueTZ;
        }
        else
        {
            //use information from previous block
            xorValue = stream->ReadBits64(64 - prevValueTZ - prevValueLZ);
            xorValue <<= prevValueTZ;
        }
        uint64_t value = xorValue ^ prevValue;
        result.push_back(*(double*)&value);
        prevValue = value;
    }
    return result;
}

}}