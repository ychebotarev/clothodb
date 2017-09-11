#pragma once

#include <string>

#include "src/TimeSeries/TimeSeriesType.h"

namespace incolun{
namespace clothodb{

struct TimeSeriesConfig
{
    std::string m_MetricName;

    TimeSeriesType m_type;

    bool m_storeMilliseconds;

    std::string m_tag1;
    std::string m_tag2;
    std::string m_tag3;
    std::string m_tag4;
    std::string m_tag5;
};

}}
