#pragma once

#include <string>

#include "src/TimeSeries/TimeSeriesType.h"

namespace incolun{
namespace clothodb{

struct TimeSeriesConfig
{
    TimeSeriesConfig()
    {
        m_type = TimeSeriesType::TypeInteger;
        m_storeMilliseconds = false;
    }

    TimeSeriesConfig(TimeSeriesConfig& config)
    {
        m_type = config.m_type;
        m_storeMilliseconds = config.m_storeMilliseconds;
        m_metricName = config.m_metricName;
        m_tag1 = config.m_tag1;
        m_tag2 = config.m_tag2;
        m_tag3 = config.m_tag3;
        m_tag4 = config.m_tag4;
        m_tag5 = config.m_tag5;
    }

    std::string m_metricName;

    TimeSeriesType m_type;

    bool m_storeMilliseconds;

    std::string m_tag1;
    std::string m_tag2;
    std::string m_tag3;
    std::string m_tag4;
    std::string m_tag5;
};

}}
