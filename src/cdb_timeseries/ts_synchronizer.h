#pragma once

#include "src/cdb_common/common.h"
#include "src/cdb_timeseries/ts_resolution.h"

namespace cdb{
namespace ts{

class ts_synchronizer
{
public:
    ts_synchronizer();
    ~ts_synchronizer();

    uint64_t get_bucket_start(uint64_t timestamp_in_ms, ts_resolution resolution);
private:

    uint64_t m_one_sec_buffer_start = 0;
    uint64_t m_five_sec_buffer_start = 0;
    uint64_t m_one_min_buffer_start = 0;
    uint64_t m_five_min_buffer_start = 0;
};

}}

