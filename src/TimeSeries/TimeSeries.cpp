#include "TimeSeries.h"

#include <functional>

#include "src/TimeSeries/Constants.h"
#include "src/TimeSeries/ErrorCodes.h"

namespace incolun{
namespace clothodb{

TimeSeries::TimeSeries(TimeSeriesConfigPtr config)
    :m_config(config),
    m_startHourInMs(0),
    m_lastTimestamp(0)
{
    ::InitializeSRWLock(&m_srwLock);
}

TimeSeries::~TimeSeries()
{}

TimeSeriesBucketPtr TimeSeries::CreateBucket()
{
    std::unique_ptr<TimeSeriesBucket> bucket(new TimeSeriesBucket(*m_config.get()));
    return bucket;
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
        MoveHeadForward();
        m_startHourInMs = FloorToHour(timestamp);
    }

    auto storedHours = ActiveBuckets();

    uint64_t tailBucketStart = (m_startHourInMs) + (storedHours - 1) * Constants::kOneHourInMs;
    if (timestamp > tailBucketStart + Constants::kOneHourInMs)
    {
        MoveHeadForward();
        tailBucketStart += Constants::kOneHourInMs;
    }

    timestamp -= tailBucketStart;
    Head().AddValue(value, (uint32_t)timestamp);
    return true;
}

TimeSeriesPoints TimeSeries::GetPoints(uint64_t startTime, uint64_t endTime)
{
    auto result = std::make_shared<std::vector<TimeSeriesPoint>>();
    if (IsEmpty()) return TimeSeriesPoints::fromValue(result);
    try
    {
        int index = 0;
        auto buckets = ActiveBuckets();

        uint64_t startHourInMs = m_startHourInMs;
        uint64_t endHourInMs = startHourInMs + Constants::kOneHourInMs;
        
        do {
            if (endHourInMs < startTime || startHourInMs >=endTime) break;

            auto& bucket = At(index);
            SRWLockConditional lock(bucket.IsSealed(), m_srwLock);
            bucket.Decompress(*result.get(), startHourInMs, startTime, endTime);
            startHourInMs += Constants::kOneHourInMs;
            endHourInMs += Constants::kOneHourInMs;
        } while (++index != buckets);
    
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
    uint64_t startHourInMs = FloorToHour(startTime);

    if (startHourInMs <= m_startHourInMs) return;

    SRWLockExclusive lock(m_srwLock);
    while (m_startHourInMs < startHourInMs && !IsEmpty())
    {
        MoveTailForward();
    }
}

uint64_t TimeSeries::FloorToHour(uint64_t timestamp)
{
    return (timestamp / Constants::kOneHourInSeconds) * Constants::kOneHourInSeconds;
}

//Buffere related funcions
inline void TimeSeries::NormalizeIndex(size_t& index) const
{
    if (index >= Constants::kMaxBuckets) index = index % Constants::kMaxBuckets;
}

size_t TimeSeries::ActiveBuckets() const
{
    if (IsEmpty()) return 0;
    return (m_headIndex >= m_tailIndex) ?
        (m_headIndex - m_tailIndex + 1) :
        (Constants::kMaxBuckets - m_tailIndex + m_headIndex + 1);
}

void TimeSeries::SetHeadIndex(size_t index)
{
    m_empty = false;
    NormalizeIndex(index);
    m_headIndex = index;
}

void TimeSeries::SetTailIndex(size_t index)
{
    m_empty = false;
    NormalizeIndex(index);
    m_tailIndex = index;
}

void TimeSeries::MoveIndexForward(size_t& index) const
{
    ++index;
    NormalizeIndex(index);
}

TimeSeriesBucket& TimeSeries::At(size_t pos)
{
    if (pos >= ActiveBuckets())
    {
        throw std::out_of_range("index our of range");
    }
    auto index = (m_tailIndex + pos) % Constants::kMaxBuckets;
    return *m_buckets[index];
}

TimeSeriesBucket& TimeSeries::Head()
{
    if (IsEmpty())
    {
        throw std::out_of_range("buffer is empty");
    }
    return *m_buckets[m_headIndex];
}

TimeSeriesBucket& TimeSeries::Tail()
{
    if (IsEmpty())
    {
        throw std::out_of_range("buffer is empty");
    }
    return *m_buckets[m_tailIndex];
}

void TimeSeries::MoveHeadForward()
{
    if (IsEmpty())
    {
        m_empty = false;
        m_headIndex = 0;
        m_tailIndex = 0;

        if (m_buckets.empty())
            m_buckets.emplace_back(new TimeSeriesBucket(*m_config.get()));
        else
            Head().Reset();
    }
    else
    {
        if (m_buckets.size() < Constants::kMaxBuckets 
            && m_headIndex == (m_buckets.size() - 1))
        {
            m_buckets.emplace_back(new TimeSeriesBucket(*m_config.get()));
        }

        //if head catch tail - move head forward
        MoveIndexForward(m_headIndex);
        Head().Reset();
        if (m_headIndex == m_tailIndex)
        {
            MoveIndexForward(m_tailIndex);
            m_startHourInMs += Constants::kOneHourInMs;
        }
    }
}

void TimeSeries::MoveTailForward()
{
    if (IsEmpty())
    {
        throw std::out_of_range("buffer is empty");
    }

    Tail().Reset();
    if (m_headIndex == m_tailIndex)
    {
        m_empty = true;
    }
    MoveIndexForward(m_tailIndex);
    m_startHourInMs += Constants::kOneHourInMs;
}

}}
