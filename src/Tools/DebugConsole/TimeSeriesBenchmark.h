#pragma once

#include <vector>
#include <memory>

#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_timeseries/time_series.h"

class TimeSeriesBenchmark
{
public:
    TimeSeriesBenchmark(
        std::shared_ptr<cdb::ts::time_series> ts,
        std::shared_ptr<cdb::ts::ts_properties> properties,
        std::vector<cdb::ts::data_point>& expected_points)
        : m_time_series(ts),
        m_properties(properties),
        m_expected_points(expected_points)
    {}

    void RunCompressTest();
    void RunDecompressTest();

private:
    std::shared_ptr<cdb::ts::time_series> m_time_series;
    std::shared_ptr<cdb::ts::ts_properties> m_properties;
    std::vector<cdb::ts::data_point>& m_expected_points;
};
