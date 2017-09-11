#pragma once

#include <string>
#include <memory>

#include "src/common/slimrwlock.h"
#include "src/common/sparsepp.h"

#include "src/TimeSeries/TimeSeriesConfig.h"

namespace incolun{
namespace clothodb{

class TimeSeriesCatalog
{
public:
    TimeSeriesCatalog();
    ~TimeSeriesCatalog();

    std::string CreateTimeSeries(std::shared_ptr<TimeSeriesConfig> config);
    std::string AddValue(std::string guid, uint64_t value, uint64_t timestamp);

    spp::sparse_hash_map<std::string, std::string> m_timeseries;
    RTL_SRWLOCK m_srwLock;
};

}}
