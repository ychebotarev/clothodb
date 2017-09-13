#include "TimeSeries.h"

#include "src/TimeSeries/Constants.h"
#include "src/TimeSeries/ErrorCodes.h"

namespace incolun{
namespace clothodb{

const int kMaxBuckets = 25;

TimeSeries::TimeSeries(std::shared_ptr<TimeSeriesConfig> config)
    :m_config(config),
    m_startHour(0),
    m_lastTimestamp(0),
    m_firstBucketIndex(0),
    m_firstEmptyIndex(0)
{
    InitializeSRWLock(&m_srwLock);
}

TimeSeries::~TimeSeries()
{
}

bool TimeSeries::IsEmpty() const
{
    return m_firstBucketIndex == m_firstEmptyIndex;
}

bool TimeSeries::AddValue(uint64_t value, uint64_t timestamp)
{
    auto timestampInSeconds = timestamp / 1000;
    if (timestampInSeconds <= m_lastTimestamp)
    {
        return false;
    }

    SRWLockExclusive lock(m_srwLock);
    m_lastTimestamp = timestampInSeconds;

    if (IsEmpty())
    {
        CreateBucket();
        m_startHour = FloorToHour(timestamp);
    }

    auto storedHours = GetStoredHours();

    uint64_t bucketStart = (m_startHour + storedHours - 1) * Constants::kOneHourInMs;
    uint64_t nextBucketStart = bucketStart + Constants::kOneHourInMs;
    if (timestamp > nextBucketStart)
    {
        CreateBucket();
        bucketStart += Constants::kOneHourInMs;
    }

    timestamp -= bucketStart;
    m_buckets[LastBucketIndex()]->AddValue(value, (uint32_t)timestamp);
    return true;
}

TimeSeriesPoints TimeSeries::GetPoints(uint64_t startTime, uint64_t endTime)
{
    TimeSeriesPoints::inner_Type result = std::make_shared<std::vector<TimeSeriesPoint>>();
    if (IsEmpty()) return TimeSeriesPoints::fromValue(result);
    try
    {
        int bucket = m_firstBucketIndex;
        uint64_t startHour = m_startHour;
        uint64_t endHour = startHour + Constants::kOneHourInMs;
        
        do {
            if (endHour < startTime || startHour >=endTime) break;

            SRWLockConditional lock(
                m_buckets[bucket]->IsSealed(),
                m_srwLock);
            m_buckets[bucket]->Decompress(*result.get(), startHour / 1000, startTime, endTime);
            startHour += Constants::kOneHourInMs;
            endHour += Constants::kOneHourInMs;
            bucket = NextBucket(bucket);
        } while (bucket != m_firstEmptyIndex);
    
        return TimeSeriesPoints::fromValue(result);
    }
    catch (const std::overflow_error&)
    {
        return TimeSeriesPoints::fromError(ErrorCodes::kBitStreamOverflaw);
    }
    catch(...)
    {
        return TimeSeriesPoints::fromError(ErrorCodes::kBitStreamUnknown);
    }
}

void TimeSeries::RemoveOldData(uint64_t startTime)
{
    uint64_t startHour = FloorToHour(startTime);

    if (startHour <= m_startHour) return;

    SRWLockExclusive lock(m_srwLock);
    while (m_startHour < startHour)
    {
        if (m_firstBucketIndex == m_firstEmptyIndex) break;
        m_buckets[m_firstBucketIndex]->Reset();

        m_firstBucketIndex = NextBucket(m_firstBucketIndex);
        m_startHour += Constants::kOneHourInMs;
    }
}

int TimeSeries::NextBucket(int bucket)
{
    ++bucket;
    while (bucket >= kMaxBuckets) bucket -= kMaxBuckets;
    return bucket;
}

int TimeSeries::LastBucketIndex()
{
    return m_firstEmptyIndex > 0 ? (m_firstEmptyIndex - 1) : (kMaxBuckets - 1);
}


void TimeSeries::CreateBucket()
{
    if (m_buckets.size() < kMaxBuckets)
    {
        m_buckets.emplace_back(new TimeSeriesBucket(*m_config.get()));
        m_firstEmptyIndex = NextBucket(m_firstEmptyIndex);
        return;
    }
    //if we catch head - move head forward
    if (m_firstEmptyIndex == m_firstBucketIndex)
        m_firstBucketIndex = NextBucket(m_firstBucketIndex);

    m_buckets[m_firstEmptyIndex]->Reset();
    m_firstEmptyIndex = NextBucket(m_firstEmptyIndex);
}

uint32_t TimeSeries::GetStoredHours()
{
    return (m_firstEmptyIndex >= m_firstBucketIndex) ?
        (m_firstEmptyIndex - m_firstBucketIndex) :
        (kMaxBuckets - m_firstBucketIndex + m_firstEmptyIndex);
}

uint64_t TimeSeries::FloorToHour(uint64_t timestamp)
{
    return (timestamp / Constants::kOneHourInSeconds) * Constants::kOneHourInSeconds;
}

}}
