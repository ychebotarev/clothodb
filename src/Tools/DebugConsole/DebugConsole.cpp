// DebugConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>
#include <time.h>

#include "src/cdb_common/time_fmt.h"
#include "src/core/time_helpers.h"
#include "src/core/time_series.h"

#include "src/cdb_compressor/compressor.h"
#include "src/cdb_compressor/decompressor.h"

#include "CompressBenchmark.h"
#include "TimeSeriesBenchmark.h"

using namespace cdb::core;

uint64_t DoubleToUint64(double value)
{
    return *((uint64_t*)&value);
}

void run_test(std::vector<ts_point>& expected_points)
{
    std::shared_ptr<ts_properties> properties = std::make_shared<ts_properties>();
    std::shared_ptr<time_series> ts = std::make_shared<time_series>(properties);

    TimeSeriesBenchmark tsb(ts, properties, expected_points);
    tsb.RunCompressTest();
    tsb.RunDecompressTest();
}

int main()
{
    const int interations = 1000000;
    srand((uint32_t)time(NULL));
    std::vector<ts_point> m_expected_points;

    uint64_t prev_timestamp = 1;
    for (int i = 0; i < interations; ++i)
    {
        uint64_t value = rand() % 1000000;
        m_expected_points.push_back({ value, prev_timestamp * 1000 });
        prev_timestamp += rand() % 3;
    }

    std::thread th1(run_test, m_expected_points);
    std::thread th2(run_test, m_expected_points);
    std::thread th3(run_test, m_expected_points);
    std::thread th4(run_test, m_expected_points);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    //{
    //    std::shared_ptr<ts_properties> properties = std::make_shared<ts_properties>();
    //    std::shared_ptr<time_series> ts = std::make_shared<time_series>(properties);
    //    
    //    TimeSeriesBenchmark tsb(ts, properties, m_expected_points);
    //    tsb.RunCompressTest();
    //    tsb.RunDecompressTest();
    //}

    //{
    //    //Timestamp compress/decompress
    //    CompressBenchmark cb(m_expected_points);
    //    {
    //        bit_stream stream(10000);
    //        cb.RunTimestampCompressTest(stream);
    //        cb.RunTimestampDecompressTest(stream);
    //    }

    //    //Integer compress/decompress
    //    {
    //        bit_stream stream(10000);
    //        cb.RunIntegerCompressTest(stream);
    //        cb.RunIntegerDecompressTest(stream);
    //    }
    //}

    //m_expected_points.clear();
    //prev_timestamp = 1;
    //for (int i = 0; i < interations; ++i)
    //{
    //    double value = rand() % 1000000;
    //    value /= 10;
    //    m_expected_points.push_back({ prev_timestamp * 1000, DoubleToUint64(value) });
    //    prev_timestamp += rand() % 3;
    //}

    //{
    //    //Double compress/decompress
    //    CompressBenchmark cb(m_expected_points);
    //    {
    //        bit_stream stream(10000);
    //        cb.RunDoubleCompressTest(stream);
    //        cb.RunDoubleDecompressTest(stream);
    //    }
    //}

    std::cin.get();
    return 0;
}

