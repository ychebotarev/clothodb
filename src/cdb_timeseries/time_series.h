#pragma once
#include "src/cdb_common/srwlock.h"

#include "src/cdb_common/common.h"
#include "src/cdb_common/serialize_block.h"

#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_timeseries/ts_bucket.h"

namespace cdb{
namespace ts{

using data_points = vector_or_error<data_point>;
using ts_bucket_ptr = std::unique_ptr<ts_bucket>;
using ts_properties_ptr = std::shared_ptr<ts_properties>;

class time_series
{
public:
    time_series(ts_properties_ptr properties);
    ~time_series();

    bool add_value(uint64_t value, uint64_t timestamp);
    
    data_points get_points();
    data_points get_points(uint64_t start_time, uint64_t end_time);

    void remove_old_data(uint64_t start_time);
    bool is_empty() const { return m_empty; };
    size_t stored_hours() const { return active_buckets_count(); }

    ts_properties_ptr get_properties() { return m_properties; }

    void serialize(cdb::serialize_block& block);
protected:
    size_t active_buckets_count() const;
private:
    ts_bucket_ptr create_bucket();
    
    //circullar buffer related functions
    void normalize_index(size_t& index) const;

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

    uint64_t m_head_start_ms;
    uint64_t m_last_timestamp_scaled;

    RTL_SRWLOCK m_srw_lock;
};

}}