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
#include "src/cdb_common/time_helpers.h"
#include "src/cdb_timeseries/time_series.h"

#include "src/cdb_compressor/compressor.h"
#include "src/cdb_compressor/decompressor.h"

#include "CompressBenchmark.h"

using namespace cdb;
using namespace cdb::ts;
using namespace cdb::compressor;
using namespace std;

uint64_t DoubleToUint64(double value)
{
    return *((uint64_t*)&value);
}

int main()
{
    const int interations = 1000000;
    srand((uint32_t)time(NULL));
    std::vector<data_point> expected_points;

    uint64_t prev_timestamp = 1;
    for (int i = 0; i < interations; ++i)
    {

        uint64_t value = 0;
        switch (rand() % 3)
        {
        case 0: value = rand() % 10;
        case 1: value = rand() % 100;
        //case 2: value = rand() % 1000;
        //case 3: value = rand() % 10000;
        }
        
        expected_points.push_back({ value, prev_timestamp * 1000 });
        ++prev_timestamp;
    }
    auto properties = make_shared<ts_properties>();
    auto ts = make_shared<time_series>(properties);
    for (auto& point : expected_points)
    {
        ts->add_value(point.value, point.timestamp);
    }

    auto points = ts->get_points();

    std::cout << "Test complete" << std::endl;
    std::cin.get();
    return 0;
}

