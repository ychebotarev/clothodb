#include "time_series.h"

#include <functional>

#include "src/cdb_common/constants.h"
#include "src/cdb_common/error_codes.h"

#include "src/core/time_helpers.h"

namespace cdb{
namespace core{

using namespace std;
using namespace cdb;

uint32_t get_hours_per_bucket(ts_scale scale)
{
    return Constants::kBucketSize 
        * time_helpers::scale_in_ms(scale)
        / Constants::kOneHourInMs;
}

uint32_t get_milliseconds_in_bucket(ts_scale scale)
{
    return Constants::kBucketSize * time_helpers::scale_in_ms(scale);
}

time_series::time_series(ts_properties_ptr properties)
    :m_properties(properties),
    m_head_start_ms(0),
    m_last_timestamp_scaled(0)
{
    ::InitializeSRWLock(&m_srw_lock);
}

time_series::~time_series()
{}

ts_bucket_ptr time_series::create_bucket()
{
    unique_ptr<ts_bucket> bucket(new ts_bucket(*m_properties.get()));
    return bucket;
}

bool time_series::add_value(uint64_t value, uint64_t timestamp)
{
    auto timestamp_scaled = time_helpers::scale_timestamp(
        timestamp, 
        m_properties->m_scale);

    if (!is_empty() && timestamp_scaled <= m_last_timestamp_scaled)
    {
        return false;
    }

    SRWLockExclusive lock(m_srw_lock);
    m_last_timestamp_scaled = timestamp_scaled;

    if (is_empty())
    {
        move_head_forward();
        m_head_start_ms = time_helpers::floor_to_hour(timestamp);
    }

    auto hours_per_bucket = get_hours_per_bucket(m_properties->m_scale);
    auto scale_in_ms = time_helpers::scale_in_ms(m_properties->m_scale);
    auto bucket_in_ms = hours_per_bucket * Constants::kOneHourInMs;
    
    uint64_t tail_bucket_start_ms = m_head_start_ms 
        + (active_buckets_count() - 1) * hours_per_bucket * Constants::kOneHourInMs;
    uint64_t tail_bucket_end_ms = tail_bucket_start_ms + hours_per_bucket * Constants::kOneHourInMs;
    
    if (timestamp_scaled * scale_in_ms > tail_bucket_end_ms)
    {
        move_head_forward();
        tail_bucket_start_ms += bucket_in_ms;
    }

    timestamp_scaled -= time_helpers::scale_timestamp(
        tail_bucket_start_ms,
        m_properties->m_scale);

    head().add_value(value, (uint32_t)timestamp_scaled, timestamp % Constants::kMillisecondsInSec);
    return true;
}

ts_points time_series::get_points()
{
    return get_points(0, 0xffffffffffffffff);
}

ts_points time_series::get_points(uint64_t start_time, uint64_t end_time)
{
    auto result = make_shared<vector<ts_point>>();
    if (is_empty()) return ts_points::from_value(result);
    //result->reserve(20000);
    try
    {
        int index = 0;
        auto buckets = active_buckets_count();
        auto bucket_length_ms = get_hours_per_bucket(m_properties->m_scale) * Constants::kOneHourInMs;

        uint64_t bucket_start_ms = m_head_start_ms;
        uint64_t bucket_end_ms = bucket_start_ms + bucket_length_ms;
        
        do {
            if (bucket_end_ms < start_time || bucket_end_ms > end_time) break;

            auto& bucket = at(index);
            SRWLockConditional lock(bucket.is_sealed(), m_srw_lock);
            bucket.decompress(*result.get(), bucket_start_ms, start_time, end_time);
            
            bucket_start_ms += bucket_length_ms;
            bucket_end_ms += bucket_length_ms;
        } while (++index != buckets);
    
        return ts_points::from_value(result);
    }
    catch (const overflow_error&)
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
    uint64_t startHourInMs = time_helpers::floor_to_hour(startTime);

    if (startHourInMs <= m_head_start_ms) return;

    SRWLockExclusive lock(m_srw_lock);
    while (m_head_start_ms < startHourInMs && !is_empty())
    {
        move_tail_forward();
    }
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
        throw out_of_range("index our of range");
    }
    auto index = (m_tail_index + pos) % Constants::kMaxBuckets;
    return *m_buckets[index];
}

ts_bucket& time_series::head()
{
    if (is_empty())
    {
        throw out_of_range("buffer is empty");
    }
    return *m_buckets[m_head_index];
}

ts_bucket& time_series::tail()
{
    if (is_empty())
    {
        throw out_of_range("buffer is empty");
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
            m_head_start_ms += get_milliseconds_in_bucket(m_properties->m_scale);
        }
    }
}

void time_series::move_tail_forward()
{
    if (is_empty())
    {
        throw out_of_range("buffer is empty");
    }

    tail().reset();
    if (m_head_index == m_tail_index)
    {
        m_empty = true;
    }
    move_index_forward(m_tail_index);
    m_head_start_ms += get_milliseconds_in_bucket(m_properties->m_scale);
}

}}
