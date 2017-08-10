#include "BitStreamDecompressor.h"

#include "BitStreamConstants.h"
#include "src/clothodb/BitUtils.h"
#include "src/clothodb/timeSeriesPoint.h"
#include "BitStream.h"

namespace incolun {
namespace clothodb {

uint32_t ReadDeltaOfDelta(const std::shared_ptr<BitStream>& stream_, int prevDoD)
{
    int deltaOfDelta = prevDoD;
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
    deltaOfDelta = BitUtils::DecodeZigZag32(deltaOfDelta);
    ++deltaOfDelta;
    return deltaOfDelta;
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

std::vector<uint32_t> BitStreamDecompressor::DecompressTimeStampts(const std::shared_ptr<BitStream>& stream)
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

}}