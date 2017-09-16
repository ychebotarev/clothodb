#pragma once

#include <string>
#include <memory>

#include "src/common/slimrwlock.h"
#include "src/common/sparsepp.h"

#include "src/core/ts_properties.h"

namespace clothodb {
namespace core {

class ts_catalog
{
public:
    ts_catalog();
    ~ts_catalog();

    std::string create_time_series(std::shared_ptr<ts_properties> config);
    std::string add_value(std::string guid, uint64_t value, uint64_t timestamp);

private:
    spp::sparse_hash_map<std::string, std::string> m_timeseries;
    RTL_SRWLOCK m_srw_lock;
};

}}
