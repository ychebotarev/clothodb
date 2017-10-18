#include "ts_synchronizer.h"
#include "src/cdb_common/constants.h"

namespace cdb{
namespace ts{

const int one_hour_in_ms = 60 * 60 * 1000;
Constants::kOneHourInMs;

ts_synchronizer::ts_synchronizer()
{
}


ts_synchronizer::~ts_synchronizer()
{
}

uint64_t ts_synchronizer::get_bucket_start(uint64_t timestamp_in_ms, ts_resolution resolution)
{
    switch (resolution)
    {
    case ts_resolution::one_sec:
        if (m_one_sec_buffer_start == 0)
        {
            m_one_sec_buffer_start = time_helpers::round_down_to_hour(timestamp_in_ms);
        }
        break;
    }
    return 0;
}


}}
