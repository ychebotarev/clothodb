#pragma once

#include "src/cdb_common/srwlock.h"
#include "src/cdb_common/sparsepp.h"
#include "src/cdb_common/common.h"

#include "src/core/ts_properties.h"

namespace cdb{
namespace core{

class time_series;
class ts_lookup;

class ts_catalog
{
public:
    ts_catalog();

    uint_or_error create_time_series(
        std::string metric,
        std::string tags,
        ts_type type = ts_type::TypeInteger,
        ts_scale scale = ts_scale::one_sec,
        bool store_milliseconds = false);

    uint_or_error create_time_series(std::shared_ptr<ts_properties> config);
    uint_or_error get_uid(std::string metric_name, tags_map tags);

    bool_or_error add_value(std::string metric_name, tags_map tags, uint64_t value, uint64_t timestamp);
    bool_or_error add_value(uint32_t uid, uint64_t value, uint64_t timestamp);

    bool_or_error delete_timeseries(std::string metric_name, tags_map tags);

private:
    spp::sparse_hash_map<uint32_t, std::shared_ptr<time_series>> m_timeseries;
    std::shared_ptr<ts_lookup> m_lookup;
    std::string m_namespace;
    RTL_SRWLOCK m_srw_lock;
};

}}
