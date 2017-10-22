#include "stdafx.h"

#include "src/cdb_timeseries/data_point.h"
#include "src/cdb_compressor/compressor.h"
#include "data_point_provider.h"

#include "src/cdb_timeseries/ts_properties.h"
#include "src/cdb_timeseries/ts_bucket.h"

#include "src/3rdparty/hayai/src/hayai.hpp"

using namespace cdb::compressor;
using namespace cdb::ts;
using namespace std;

class CompressBenchmarkFixture
    : public ::hayai::Fixture
{
public:
    virtual void SetUp()
    {
        auto& data_points = data_point_provider::get_points();
        m_stream = std::shared_ptr<bit_stream>(new bit_stream(1000000));
    }

    virtual void TearDown()
    {}

    void RunTimestampCompressTest()
    {
        auto& data_points = data_point_provider::get_points();
        bit_stream_writer writer(*m_stream.get());
        timestamp_compressor compressor(writer);

        compressor.append_first_value(0);
        for (auto& point : data_points)
        {
            compressor.append_next_value((uint32_t)point.timestamp);
        }
    }

    void RunIntegerCompressTest()
    {
        auto& data_points = data_point_provider::get_points();
        bit_stream_writer writer(*m_stream.get());
        integer_compressor compressor(writer);

        compressor.append_first_value(0);
        for (auto& point : data_points)
        {
            compressor.append_next_value(point.value);
        }
    }

    void RunTimestampAndIntegerCompressTest()
    {
        shared_ptr<bit_stream> stream(new bit_stream(1000000));

        RunTimestampAndIntegerCompressTest(*stream.get());
    }
    
    void RunTimestampAndIntegerCompressTestPreAllocate()
    {
        RunTimestampAndIntegerCompressTest(*m_stream.get());
    }

    void RunTimestampAndIntegerCompressTest(bit_stream& stream)
    {
        auto& data_points = data_point_provider::get_points();
        bit_stream_writer writer(stream);

        timestamp_compressor timestampCompressor(writer);
        integer_compressor integerCompressor(writer);

        for (auto& point : data_points)
        {
            bool first_value = stream.is_empty();

            if (first_value)
            {
                timestampCompressor.append_first_value((uint32_t)point.timestamp / 1000);
                integerCompressor.append_first_value(point.value);
            }
            else
            {
                timestampCompressor.append_next_value((uint32_t)point.timestamp / 1000);
                integerCompressor.append_next_value(point.value);
            }

            writer.commit();
        }
    }

protected:
    shared_ptr<bit_stream> m_stream;
};

//BENCHMARK_F(CompressBenchmarkFixture, TimestampCompressTest, 5, 20)
//{
//    RunTimestampCompressTest();
//}
//
//BENCHMARK_F(CompressBenchmarkFixture, IntegerCompressTest, 5, 20)
//{
//    RunIntegerCompressTest();
//}
//

//BENCHMARK_F(CompressBenchmarkFixture, TimestampAndIntegerCompressTest1, 2, 10)
//{
//    RunTimestampAndIntegerCompressTest();
//}
//
//BENCHMARK_F(CompressBenchmarkFixture, TimestampAndIntegerCompressTestPreAllocate, 2, 10)
//{
//    RunTimestampAndIntegerCompressTestPreAllocate();
//}

