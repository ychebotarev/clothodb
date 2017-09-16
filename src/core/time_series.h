#pragma once
#include "src/common/slimrwlock.h"

#include "src/core/common.h"

#include "src/core/ts_properties.h"
#include "src/core/ts_bucket.h"

namespace clothodb {
namespace core {

using ts_points = vector_or_error<ts_point>;
using ts_bucket_ptr = std::unique_ptr<ts_bucket>;
using ts_properties_ptr = std::shared_ptr<ts_properties>;

class time_series
{
public:
    time_series(ts_properties_ptr properties);
    ~time_series();

    bool add_value(uint64_t value, uint64_t timestamp);
    ts_points get_points(uint64_t start_time, uint64_t end_time);
    void remove_old_data(uint64_t start_time);
    bool is_empty() const { return m_empty; };
    size_t stored_hours() const { return active_buckets_count(); }
private:
    ts_bucket_ptr create_bucket();
    
    uint64_t floor_to_hour(uint64_t timestamp);

    //circullar buffer related functions
    void normalize_index(size_t& index) const;

    size_t active_buckets_count() const;

    void set_head_index(size_t index);
    void set_tail_index(size_t index);

    ts_bucket& head();
    ts_bucket& tail();
    ts_bucket& at(size_t pos);

    void move_index_forward(size_t& index) const;
    void move_head_forward();
    void move_tail_forward();

private:
    std::vector<ts_bucket_ptr> m_buckets;

    size_t m_head_index = 0;
    size_t m_tail_index = 0;
    bool m_empty = true;

    ts_properties_ptr m_properties;

    uint64_t m_start_hour_ms;
    uint64_t m_last_timestamp;

    RTL_SRWLOCK m_srw_lock;
};

}}