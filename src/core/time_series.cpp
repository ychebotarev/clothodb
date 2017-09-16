#include "time_series.h"

#include <functional>

#include "src/core/Constants.h"
#include "src/core/ErrorCodes.h"

namespace clothodb {
namespace core {

time_series::time_series(ts_properties_ptr properties)
    :m_properties(properties),
    m_start_hour_ms(0),
    m_last_timestamp(0)
{
    ::InitializeSRWLock(&m_srw_lock);
}

time_series::~time_series()
{}

ts_bucket_ptr time_series::create_bucket()
{
    std::unique_ptr<ts_bucket> bucket(new ts_bucket(*m_properties.get()));
    return bucket;
}

bool time_series::add_value(uint64_t value, uint64_t timestamp)
{
    auto timestamp_sec = timestamp / 1000;
    if (timestamp_sec <= m_last_timestamp)
    {
        return false;
    }

    SRWLockExclusive lock(m_srw_lock);
    m_last_timestamp = timestamp_sec;

    if (is_empty())
    {
        move_head_forward();
        m_start_hour_ms = floor_to_hour(timestamp);
    }

    auto stored_hours = active_buckets_count();

    uint64_t tail_bucket_start = (m_start_hour_ms) + (stored_hours - 1) * Constants::kOneHourInMs;
    if (timestamp > tail_bucket_start + Constants::kOneHourInMs)
    {
        move_head_forward();
        tail_bucket_start += Constants::kOneHourInMs;
    }

    timestamp -= tail_bucket_start;
    head().add_value(value, (uint32_t)timestamp);
    return true;
}

ts_points time_series::get_points(uint64_t start_time, uint64_t end_time)
{
    auto result = std::make_shared<std::vector<ts_point>>();
    if (is_empty()) return ts_points::from_value(result);
    try
    {
        int index = 0;
        auto buckets = active_buckets_count();

        uint64_t start_hour_ms = m_start_hour_ms;
        uint64_t endHourInMs = start_hour_ms + Constants::kOneHourInMs;
        
        do {
            if (endHourInMs < start_time || start_hour_ms >=end_time) break;

            auto& bucket = at(index);
            SRWLockConditional lock(bucket.is_sealed(), m_srw_lock);
            bucket.decompress(*result.get(), start_hour_ms, start_time, end_time);
            start_hour_ms += Constants::kOneHourInMs;
            endHourInMs += Constants::kOneHourInMs;
        } while (++index != buckets);
    
        return ts_points::from_value(result);
    }
    catch (const std::overflow_error&)
    {
        return ts_points::from_error(ErrorCodes::kBitStreamOverflaw);
    }
    catch(...)
    {
        return ts_points::from_error(ErrorCodes::kBitStreamUnknown);
    }
}

void time_series::remove_old_data(uint64_t startTime)
{
    uint64_t startHourInMs = floor_to_hour(startTime);

    if (startHourInMs <= m_start_hour_ms) return;

    SRWLockExclusive lock(m_srw_lock);
    while (m_start_hour_ms < startHourInMs && !is_empty())
    {
        move_tail_forward();
    }
}

uint64_t time_series::floor_to_hour(uint64_t timestamp)
{
    return (timestamp / Constants::kOneHourInSeconds) * Constants::kOneHourInSeconds;
}

//Buffere related funcions
inline void time_series::normalize_index(size_t& index) const
{
    if (index >= Constants::kMaxBuckets) index = index % Constants::kMaxBuckets;
}

size_t time_series::active_buckets_count() const
{
    if (is_empty()) return 0;
    return (m_head_index >= m_tail_index) ?
        (m_head_index - m_tail_index + 1) :
        (Constants::kMaxBuckets - m_tail_index + m_head_index + 1);
}

void time_series::set_head_index(size_t index)
{
    m_empty = false;
    normalize_index(index);
    m_head_index = index;
}

void time_series::set_tail_index(size_t index)
{
    m_empty = false;
    normalize_index(index);
    m_tail_index = index;
}

void time_series::move_index_forward(size_t& index) const
{
    ++index;
    normalize_index(index);
}

ts_bucket& time_series::at(size_t pos)
{
    if (pos >= active_buckets_count())
    {
        throw std::out_of_range("index our of range");
    }
    auto index = (m_tail_index + pos) % Constants::kMaxBuckets;
    return *m_buckets[index];
}

ts_bucket& time_series::head()
{
    if (is_empty())
    {
        throw std::out_of_range("buffer is empty");
    }
    return *m_buckets[m_head_index];
}

ts_bucket& time_series::tail()
{
    if (is_empty())
    {
        throw std::out_of_range("buffer is empty");
    }
    return *m_buckets[m_tail_index];
}

void time_series::move_head_forward()
{
    if (is_empty())
    {
        m_empty = false;
        m_head_index = 0;
        m_tail_index = 0;

        if (m_buckets.empty())
            m_buckets.emplace_back(new ts_bucket(*m_properties.get()));
        else
            head().reset();
    }
    else
    {
        if (m_buckets.size() < Constants::kMaxBuckets 
            && m_head_index == (m_buckets.size() - 1))
        {
            m_buckets.emplace_back(new ts_bucket(*m_properties.get()));
        }

        //if head catch tail - move head forward
        move_index_forward(m_head_index);
        head().reset();
        if (m_head_index == m_tail_index)
        {
            move_index_forward(m_tail_index);
            m_start_hour_ms += Constants::kOneHourInMs;
        }
    }
}

void time_series::move_tail_forward()
{
    if (is_empty())
    {
        throw std::out_of_range("buffer is empty");
    }

    tail().reset();
    if (m_head_index == m_tail_index)
    {
        m_empty = true;
    }
    move_index_forward(m_tail_index);
    m_start_hour_ms += Constants::kOneHourInMs;
}

}}
