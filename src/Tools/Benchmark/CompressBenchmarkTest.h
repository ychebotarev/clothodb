#pragma once

#include "src/cdb_timeseries/data_point.h"

#undef min
#undef max
#include "src/3rdparty/hayai/src/hayai.hpp"

class CompressBenchmarkTest
{
public:
    CompressBenchmarkTest(std::vector<cdb::ts::data_point>& expected_points)
        : m_expected_points(expected_points)
    {}

    void RunTimestampCompressTest();
    void RunIntegerCompressTest();

private:
    std::vector<cdb::ts::data_point>& m_expected_points;
};

class CompressBenchmarkFixture
    : public ::hayai::Fixture
{
public:
    virtual void SetUp();

    virtual void TearDown()
    {
        delete this->m_test;
    }
public:
    CompressBenchmarkTest* m_test;

protected:
    std::vector<cdb::ts::data_point> m_expected_points;
};


