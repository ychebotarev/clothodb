#include "stdafx.h"
#include <stdlib.h>

#include "CompressBenchmarkTest.h"
#include "src/cdb_compressor/compressor.h"

using namespace cdb::compressor;

void CompressBenchmarkTest::RunTimestampCompressTest()
{
    bit_stream stream(10000000);
    bit_stream_writer writer(stream);
    timestamp_compressor compressor(writer);

    compressor.append_first_value(0);
    for (auto& point : m_expected_points)
    {
        compressor.append_next_value((uint32_t)point.timestamp);
    }
}

void CompressBenchmarkTest::RunIntegerCompressTest()
{
    bit_stream stream(10000000);
    bit_stream_writer writer(stream);
    integer_compressor compressor(writer);

    compressor.append_first_value(0);
    for (auto& point : m_expected_points)
    {
        compressor.append_next_value(point.value);
    }
}

void CompressBenchmarkFixture::SetUp()
{
    srand((uint32_t)time(NULL));
    m_expected_points.clear();

    uint64_t prev_timestamp = 1;
    for (int i = 0; i < 1000000; ++i)
    {
        uint64_t value = rand() % 1000000;
        m_expected_points.push_back({ prev_timestamp * 1000, value });
        prev_timestamp += rand() % 3;
    }
    m_test = new CompressBenchmarkTest(m_expected_points);
}

BENCHMARK_F(CompressBenchmarkFixture, RunTimestampCompressTest, 5, 100)
{
    m_test->RunTimestampCompressTest();
}

BENCHMARK_F(CompressBenchmarkFixture, RunIntegerCompressTest, 5, 100)
{
    m_test->RunIntegerCompressTest();
}
