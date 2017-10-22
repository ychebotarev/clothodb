#include "stdafx.h"
#include <vector>

#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_timeseries/time_series.h"
#include "data_point_provider.h"

#undef min
#undef max
#include "src/3rdparty/hayai/src/hayai.hpp"

using namespace std;
using namespace cdb::ts;
using namespace cdb::compressor;

class TimeSeriesBenchmarkFixture
    : public ::hayai::Fixture
{
public:
    virtual void SetUp()
    {
        auto& data_points = data_point_provider::get_points();
        auto properties = make_shared<ts_properties>();
        m_ts = make_shared<time_series>(properties);
        for (auto& point : data_points)
        {
            m_ts->add_value(point.value, point.timestamp);
        }
    }

    virtual void TearDown()
    {}

    void RunCompressTest()
    {
        auto& data_points = data_point_provider::get_points();
        auto properties = make_shared<ts_properties>();
        auto ts = make_shared<time_series>(properties);
        for (auto& point : data_points)
        {
            ts->add_value(point.value, point.timestamp);
        }
    }

    void RunDecompressTest()
    {
        size_t decompressed_points = 0;
        for (int i = 0; i < 100; ++i)
        {
            auto result = m_ts->get_points();
            decompressed_points += result.value->size();
        }
    }

private:
    shared_ptr<time_series> m_ts;
};

BENCHMARK_F(TimeSeriesBenchmarkFixture, TimeSeriesCompressTest, 2, 10)
{
    RunCompressTest();
}

//BENCHMARK_F(TimeSeriesBenchmarkFixture, TimeSeriesDecompressTest, 1, 5)
//{
//    RunDecompressTest();
//}
