#include "stdafx.h"
#include "LoadTimeSeriesBenchmark.h"
#include "src/cdb_common/time_fmt.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
using namespace cdb::core;

using vector_of_strings = shared_ptr<vector<string>>;
using split_vector_type = vector<string>;

vector_of_strings get_all_lines(string file_name)
{
    ifstream infile(file_name);
    string line;
    vector_of_strings lines = make_shared<vector<string>>();
    while (std::getline(infile, line))
    {
        lines->push_back(line);
    }
    return lines;
}

void LoadTimeSeriesBenchmarkFixture::SetUp()
{
    cdb::time_fmt converter("%Y-%m-%d %H:%M:%S");

    m_properties = make_shared<ts_properties>();
    m_properties->m_metric = "elb_request_count_8c0756";
    m_properties->m_scale = ts_scale::five_min;
    m_expected_points.clear();

    auto lines = get_all_lines("..\\benchmark\\data\\realAWSCloudwatch\\elb_request_count_8c0756.csv");
    for (auto& line : *lines.get())
    {
        split_vector_type split_vec;
        boost::split(split_vec, line, boost::is_any_of(","), boost::token_compress_on);

        if (split_vec.size() != 2) continue;

        auto timestamp = converter.from_string(split_vec[0].c_str());
        if (timestamp == -1) continue;

        auto value = (uint64_t)stod(split_vec[1]);

        m_expected_points.push_back({ value, (uint64_t)timestamp });
    }

    m_time_series = make_shared<time_series>(m_properties);
    for (auto& point : m_expected_points)
    {
        m_time_series->add_value(point.value, point.timestamp);
    }

    this->m_timeSeriesTest = new LoadTimeSeriesBenchmarkTest(
        m_time_series,
        m_properties,
        m_expected_points);
}

void LoadTimeSeriesBenchmarkTest::RunCompressTest()
{
    time_series ts(m_properties);
    for (auto& point : m_expected_points)
    {
        ts.add_value(point.value, point.timestamp);
    }
}

void LoadTimeSeriesBenchmarkTest::RunDecompressTest()
{
    auto result = m_time_series->get_points();
}


#if 0
BENCHMARK_F(LoadTimeSeriesBenchmarkFixture, RunCompressTest, 10, 100)
{
    m_timeSeriesTest->RunCompressTest();
}

BENCHMARK_F(LoadTimeSeriesBenchmarkFixture, RunDecompressTest, 10, 100)
{
    m_timeSeriesTest->RunDecompressTest();
}
#endif