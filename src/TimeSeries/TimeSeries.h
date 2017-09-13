#pragma once
#include "src/common/slimrwlock.h"

#include "src/TimeSeries/common.h"

#include "src/TimeSeries/timeseriesconfig.h"
#include "src/TimeSeries/timeseriesbucket.h"

namespace incolun{
namespace clothodb{

using TimeSeriesPoints = VectorOrError<TimeSeriesPoint>;

class TimeSeries
{
public:
    TimeSeries(std::shared_ptr<TimeSeriesConfig> config);
    ~TimeSeries();

    bool AddValue(uint64_t value, uint64_t timestamp);
    TimeSeriesPoints GetPoints(uint64_t startTime, uint64_t endTime);
    void RemoveOldData(uint64_t startTime);
    bool IsEmpty() const;

private:
    void CreateBucket();
    uint32_t GetStoredHours();
    uint64_t FloorToHour(uint64_t timestamp);

    int NextBucket(int bucket);
    int LastBucketIndex();
private:
    std::shared_ptr<TimeSeriesConfig> m_config;
    std::vector<std::unique_ptr<TimeSeriesBucket>> m_buckets;
    int m_firstBucketIndex;
    int m_firstEmptyIndex;
    uint64_t m_startHour;
    uint64_t m_lastTimestamp;
    RTL_SRWLOCK m_srwLock;
};

}}

