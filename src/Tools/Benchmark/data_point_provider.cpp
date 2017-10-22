#include "stdafx.h"
#include "data_point_provider.h"
#include <chrono>
#include <mutex>

using namespace std;
using namespace cdb::ts;

vector<data_point> expected_points;
mutex m_mutex;

vector<data_point>& data_point_provider::get_points()
{
    if (!expected_points.empty())
        return expected_points;

    lock_guard<mutex> lock(m_mutex);
    if (!expected_points.empty())
        return expected_points;

    auto ms_since_epoch =
        chrono::system_clock::now().time_since_epoch() /
        chrono::milliseconds(1);

    srand((unsigned int)ms_since_epoch);

    uint64_t prev_timestamp = 1;
    for (int i = 0; i < 1000000; ++i)
    {
        uint64_t value = rand() % 1000000;
        expected_points.push_back({ value, prev_timestamp * 1000 });
        prev_timestamp += rand() % 3;
    }
    return expected_points;
}