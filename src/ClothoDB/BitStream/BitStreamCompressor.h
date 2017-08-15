#pragma once

#include <stdint.h>
#include <limits>
#include <vector>
#include <string>
#include <memory>

namespace incolun{
namespace clothodb{

class TimeSeriesPoint;
class BitStream;

class BitStreamCompressor
{
public:
    BitStreamCompressor(std::shared_ptr<BitStream> stream);
    ~BitStreamCompressor();

    bool Append(const TimeSeriesPoint& dataPoint);
    bool Append(int32_t timestamp, double value);

	static std::shared_ptr<BitStream> CompressTimestamps(const std::vector<uint32_t>& timestamps);
    static std::shared_ptr<BitStream> CompressValues(const std::vector<double>& values);
protected:
    void AppendTimestamp(int32_t timestamp);
    void AppendValue(double value);

private:
    std::shared_ptr<BitStream> stream_;

    int32_t startTime_;

    uint32_t prevTimestamp_;
    uint32_t prevTimestampDelta_;

    uint64_t prevValue_;
    uint32_t prevValueTZ_;
    uint32_t prevValueLZ_;
};

}}

