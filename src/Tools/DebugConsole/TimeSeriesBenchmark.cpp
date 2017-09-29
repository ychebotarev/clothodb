#include "stdafx.h"
#include "TimeSeriesBenchmark.h"

#include <iostream>
#include <chrono>

void TimeSeriesBenchmark::RunCompressTest()
{
    auto started = std::chrono::high_resolution_clock::now();
    for (auto& point : m_expected_points)
    {
        m_time_series->add_value(point.value, point.timestamp);
    }
    auto done = std::chrono::high_resolution_clock::now();
    auto ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << "TimeSeries compression: " << ellapsed << " ms." << std::endl;
}

void TimeSeriesBenchmark::RunDecompressTest()
{
    auto started = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 125; ++i)
    {
        auto result = m_time_series->get_points();
    }
    auto done = std::chrono::high_resolution_clock::now();
    auto ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << "TimeSeries decompression: " << ellapsed << " ms." << std::endl;
}
