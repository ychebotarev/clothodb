#pragma once

#include "src/TimeSeries/common.h"

#include "src/TimeSeries/bitstream.h"
#include "src/TimeSeries/TimeSeriesPoint.h"
#include "src/TimeSeries/TimeSeriesConfig.h"

#include "src/TimeSeries/Compressor/Compressor.h"

namespace incolun{
namespace clothodb{

class TimeSeriesBucket
{
public:
    TimeSeriesBucket(const TimeSeriesConfig& config);
    ~TimeSeriesBucket();

    void Reset();
    void AddValue(uint64_t value, uint32_t timestamp);

    void Decompress(std::vector<TimeSeriesPoint>& points, 
        uint64_t baseTime,
        uint64_t startTime,
        uint64_t endTime);

    void Seal();
    bool IsSealed() const { return m_Sealed; };

    std::shared_ptr<BitStream> GetStream() const { return m_stream; }
private:
    std::shared_ptr<BitStream> m_stream;
    const TimeSeriesConfig& m_config;

    std::unique_ptr<TimestampCompressor> m_timeStampCompressor;
    std::unique_ptr<ValueCompressor> m_valueCompressor;
    bool m_Sealed;
};

}}