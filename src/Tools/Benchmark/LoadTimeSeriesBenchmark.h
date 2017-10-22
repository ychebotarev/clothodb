#pragma once

#include <fstream>
#include <iostream>

#include "src/cdb_common/time_helpers.h"
#include "src/cdb_timeseries/time_series.h"

#undef min
#undef max
#include "src/3rdparty/hayai/src/hayai.hpp"

class LoadTimeSeriesBenchmarkTest
{
public:
    LoadTimeSeriesBenchmarkTest(
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

class LoadTimeSeriesBenchmarkFixture
    : public ::hayai::Fixture
{
public:
    virtual void SetUp();

    virtual void TearDown()
    {
        delete this->m_timeSeriesTest;
    }
public:
    LoadTimeSeriesBenchmarkTest* m_timeSeriesTest;

protected:
    std::shared_ptr<cdb::ts::time_series> m_time_series;
    std::shared_ptr<cdb::ts::ts_properties> m_properties;
    std::vector<cdb::ts::data_point>& m_expected_points;
};


