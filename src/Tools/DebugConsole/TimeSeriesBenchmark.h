#pragma once

#include <vector>
#include <memory>

#include "src/core/ts_properties.h"
#include "src/core/time_series.h"

class TimeSeriesBenchmark
{
public:
    TimeSeriesBenchmark(
        std::shared_ptr<cdb::core::time_series> ts,
        std::shared_ptr<cdb::core::ts_properties> properties,
        std::vector<cdb::core::ts_point>& expected_points)
        : m_time_series(ts),
        m_properties(properties),
        m_expected_points(expected_points)
    {}

    void RunCompressTest();
    void RunDecompressTest();

private:
    std::shared_ptr<cdb::core::time_series> m_time_series;
    std::shared_ptr<cdb::core::ts_properties> m_properties;
    std::vector<cdb::core::ts_point>& m_expected_points;
};
