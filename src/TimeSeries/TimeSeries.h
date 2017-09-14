#pragma once
#include "src/common/slimrwlock.h"
#include "src/common/circularbuffer.h"

#include "src/TimeSeries/common.h"

#include "src/TimeSeries/timeseriesconfig.h"
#include "src/TimeSeries/timeseriesbucket.h"

namespace incolun{
namespace clothodb{

using TimeSeriesPoints = VectorOrError<TimeSeriesPoint>;
using TimeSeriesBucketPtr = std::unique_ptr<TimeSeriesBucket>;
using TimeSeriesConfigPtr = std::shared_ptr<TimeSeriesConfig>;

class TimeSeries
{
public:
    TimeSeries(TimeSeriesConfigPtr config);
    ~TimeSeries();

    bool AddValue(uint64_t value, uint64_t timestamp);
    TimeSeriesPoints GetPoints(uint64_t startTime, uint64_t endTime);
    void RemoveOldData(uint64_t startTime);
    bool IsEmpty() const { return m_empty; };
    size_t StoredHours() const { return ActiveBuckets(); }
private:
    TimeSeriesBucketPtr CreateBucket();
    
    uint64_t FloorToHour(uint64_t timestamp);

    //circullar buffer related functions
    void NormalizeIndex(size_t& index) const;

    size_t TimeSeries::ActiveBuckets() const;

    void SetHeadIndex(size_t index);
    void SetTailIndex(size_t index);

    TimeSeriesBucket& Head();
    TimeSeriesBucket& Tail();
    TimeSeriesBucket& At(size_t pos);

    void MoveIndexForward(size_t& index) const;
    void MoveHeadForward();
    void MoveTailForward();

private:
    std::vector<TimeSeriesBucketPtr> m_buckets;

    size_t m_headIndex = 0;
    size_t m_tailIndex = 0;
    bool m_empty = true;

    TimeSeriesConfigPtr m_config;

    uint64_t m_startHourInMs;
    uint64_t m_lastTimestamp;

    RTL_SRWLOCK m_srwLock;
};

}}

