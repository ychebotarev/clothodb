#include "stdafx.h"
#include <vector>

#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_timeseries/ts_bucket.h"
#include "data_point_provider.h"

#undef min
#undef max
#include "src/3rdparty/hayai/src/hayai.hpp"

using namespace std;
using namespace cdb::ts;
using namespace cdb::compressor;

class BucketBenchmarkFixture
    : public ::hayai::Fixture
{
public:
    virtual void SetUp()
    {
        auto& data_points = data_point_provider::get_points();
        m_properties = make_shared<ts_properties>();
    }

    virtual void TearDown()
    {}

    void RunCompressTest()
    {
        auto& data_points = data_point_provider::get_points();
        auto bucket = make_shared<ts_bucket>(*m_properties.get());

        for (auto& point : data_points)
        {
            bucket->add_value(point.value, (uint32_t)(point.timestamp / 1000), 0);
        }
    }

private:
    shared_ptr<ts_properties> m_properties;
};

BENCHMARK_F(BucketBenchmarkFixture, BucketCompressTest, 2, 10)
{
    RunCompressTest();
}
