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
    m_startBucket(0),
    m_endBucket(0)
{
    InitializeSRWLock(&m_srwLock);
}

TimeSeries::~TimeSeries()
{
}

bool TimeSeries::IsEmpty() const
{
    return m_startBucket == m_endBucket;
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
    uint64_t nextBucketStart = m_startHour + storedHours;
    if (timestamp > nextBucketStart)
    {
        CreateBucket();
        storedHours += Constants::kOneHourInSeconds;
    }

    timestamp -= m_startHour;
    timestamp -= storedHours;
    m_buckets[m_endBucket]->AddValue(value, (uint32_t)timestamp);
    return true;
}

TimeSeriesPoints TimeSeries::GetPoints(uint64_t startTime, uint64_t endTime)
{
    TimeSeriesPoints::inner_Type result = std::make_shared<std::vector<TimeSeriesPoint>>();
    if (m_startBucket == -1) return TimeSeriesPoints::fromValue(result);
    try
    {
        int bucket = m_startBucket;
        uint64_t startHour = m_startHour;
        uint64_t endHour = startHour + Constants::kOneHourInMs;
        
        do {
            if (endHour < startTime || startHour >=endTime) break;

            uint32_t toDecompress = 0;
            if (m_buckets[bucket]->IsSealed())
            {
                toDecompress = m_buckets[bucket]->GetStream()->GetLength();
            }
            else
            {
                SRWLockShared lock(m_srwLock);
                toDecompress = m_buckets[bucket]->GetStream()->GetPosition();

            }
            m_buckets[bucket]->Decompress(toDecompress, *result.get(), startHour, startTime, endTime);
            startHour += Constants::kOneHourInMs;
            endHour += Constants::kOneHourInMs;
            ++bucket;
            if (bucket >= kMaxBuckets) bucket -= kMaxBuckets;
        } while (bucket != m_endBucket);
    
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
        if (m_startBucket == m_endBucket) break;
        m_buckets[m_startBucket]->Reset();

        m_startBucket = NextBucket(m_startBucket);
        m_startHour += Constants::kOneHourInMs;
    }
}

int TimeSeries::NextBucket(int bucket)
{
    ++bucket;
    while (bucket >= kMaxBuckets) bucket -= kMaxBuckets;
}

void TimeSeries::CreateBucket()
{
    if (m_buckets.size() < kMaxBuckets)
    {
        m_buckets.emplace_back(new TimeSeriesBucket(*m_config.get()));
        m_endBucket = (int)m_buckets.size();
        return;
    }
    m_endBucket = NextBucket(m_endBucket);
    m_buckets[m_endBucket]->Reset();

    //if we catch head - move head forward
    if (m_endBucket == m_startBucket) 
        m_startBucket = NextBucket(m_startBucket);
}

uint32_t TimeSeries::GetStoredHours()
{
    return (m_endBucket >= m_startBucket) ?
        (m_endBucket - m_startBucket) :
        (kMaxBuckets - m_startBucket + m_endBucket);
}

uint64_t TimeSeries::FloorToHour(uint64_t timestamp)
{
    return (timestamp / Constants::kOneHourInSeconds) * Constants::kOneHourInSeconds;
}

}}
