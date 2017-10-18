#pragma once

#include "src/cdb_common/common.h"

namespace cdb{
namespace db{

struct timeseries_description
{
    std::string m_namespace_name;
    std::string m_metric_name;
    std::map<std::string, std::string>& m_tags;
};

}}

