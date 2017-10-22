#include "stdafx.h"

#include <functional>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/cdb_common/constants.h"
#include "src/cdb_common/time_helpers.h"
#include "src/cdb_timeseries/time_series.h"

using namespace cdb;
using namespace cdb::ts;

namespace ClothDBTest
{
    TEST_CLASS(TimeSeriesTests)
    {
    public:
        TEST_METHOD(TimeSeriesTestSimple)
        {
            auto properties = CreateProperties();
            time_series timeSeries(properties);
            std::vector<data_point> expected_points{ { 1, 1000 },{ 2, 2000 } };
            for (auto &point : expected_points)
            {
                timeSeries.add_value(point.value, point.timestamp);
            }

            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }

        TEST_METHOD(TimeSeriesTestEmpty)
        {
            auto properties = CreateProperties();
            time_series timeSeries(properties);

            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            Assert::IsTrue(actual_points->size() == 0);
        }

        TEST_METHOD(TimeSeriesTestSimpleInteger)
        {
            auto properties = CreateProperties();
            time_series timeSeries(properties);
            std::vector<data_point> expected_points{ { 0, 1000 } };
            for (auto &point : expected_points)
            {
                timeSeries.add_value(point.value, point.timestamp);
            }
            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }
        
        TEST_METHOD(TimeSeriesTestSimpleDouble)
        {
            auto properties = CreateProperties();
            properties->m_type = ts_type::TypeDouble;
            time_series timeSeries(properties);
            std::vector<data_point> expected_points{ { 0, 1000 } };
            for (auto &point : expected_points)
            {
                timeSeries.add_value(point.value, point.timestamp);
            }
            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }
        
        TEST_METHOD(TimeSeriesTestSimpleMilliseconds)
        {
            auto properties = CreateProperties();
            properties->m_type = ts_type::TypeDouble;
            properties->m_store_milliseconds = true;
            time_series timeSeries(properties);
            std::vector<data_point> expected_points{ { 0, 1001 } };
            for (auto &point : expected_points)
            {
                timeSeries.add_value(point.value, point.timestamp);
            }

            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }

        TEST_METHOD(TimeSeriesTestSimpleNoMilliseconds)
        {
            auto properties = CreateProperties();
            properties->m_type = ts_type::TypeDouble;
            time_series timeSeries(properties);
            std::vector<data_point> expected_points{ { 0, 1000 } };
            for (auto &point : expected_points)
            {
                timeSeries.add_value(point.value, point.timestamp);
            }
            
            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }

        TEST_METHOD(TimeSeriesTestMultipleBucketsSimple)
        {
            auto properties = CreateProperties();
            properties->m_type = ts_type::TypeInteger;
            time_series timeSeries(properties);
            uint64_t startTime = 1000;
            std::vector<data_point> expected_points;
            for (int hour = 0; hour < 2; ++hour)
            {
                for (int minute = 0; minute < 60; ++minute)
                {
                    uint64_t totalMintes = hour * 60 + minute;
                    uint64_t timestamp = startTime + totalMintes * 60 * 1000;
                    expected_points.push_back({ totalMintes, timestamp });
                }
            }
            for (auto &point : expected_points)
            {
                timeSeries.add_value(point.value, point.timestamp);
            }

            Assert::AreEqual(2, (int)timeSeries.stored_hours());
            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }

        TEST_METHOD(TimeSeriesTestMultipleBucketsOverflaw)
        {
            auto properties = CreateProperties();
            properties->m_type = ts_type::TypeInteger;
            time_series timeSeries(properties);
            uint64_t startTime = 1000;

            std::vector<data_point> expected_points;
            for (int hour = 0; hour <= Constants::kMaxBuckets; ++hour)
            {
                for (int minute = 0; minute < 60; ++minute)
                {
                    uint64_t totalMintes = hour * 60 + minute;
                    uint64_t timestamp = startTime + totalMintes * 60 * 1000;
                    timeSeries.add_value(totalMintes, timestamp);
                    if(hour >= 1)
                        expected_points.push_back({ totalMintes, timestamp });
                }
            }
            auto storedHours = timeSeries.stored_hours();
            Assert::AreEqual(Constants::kMaxBuckets, storedHours);
            auto result = timeSeries.get_points();
            
            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }
        
        TEST_METHOD(TimeSeriesTestRemoveOldData)
        {
            auto properties = CreateProperties();
            properties->m_type = ts_type::TypeInteger;
            time_series timeSeries(properties);
            uint64_t startTime = 1000;

            std::vector<data_point> expected_points;
            for (int hour = 0; hour <= Constants::kMaxBuckets; ++hour)
            {
                for (int minute = 0; minute < 60; ++minute)
                {
                    uint64_t totalMintes = hour * 60 + minute;
                    uint64_t timestamp = startTime + totalMintes * 60 * 1000;
                    timeSeries.add_value(totalMintes, timestamp);
                    if (hour >= 3)
                        expected_points.push_back({ totalMintes, timestamp });
                }
            }
            auto oldTime = 3 * Constants::kOneHourInMs;
            timeSeries.remove_old_data(oldTime);
            auto storedHours = timeSeries.stored_hours();
            Assert::AreEqual(Constants::kMaxBuckets - 2, storedHours);
            
            auto result = timeSeries.get_points();
            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }        

        TEST_METHOD(time_series_tests_resolution)
        {
            time_series_tests_resolution(ts_resolution::one_sec);
            time_series_tests_resolution(ts_resolution::one_min);
            time_series_tests_resolution(ts_resolution::five_sec);
            time_series_tests_resolution(ts_resolution::five_min);
        }

        void time_series_tests_resolution(ts_resolution resolution)
        {
            auto properties = CreateProperties();
            properties->m_resolution = resolution;
            time_series timeSeries(properties);
            auto scale_in_ms = (int)(properties->m_resolution);
            std::vector<data_point> expected_points{ { 0, (uint64_t)2 * scale_in_ms } };
            for (auto &point : expected_points)
            {
                timeSeries.add_value(point.value, point.timestamp + 10);
            }
            bool success = timeSeries.add_value(4, 2 * scale_in_ms - 1);
            Assert::IsFalse(success);
            
            success = timeSeries.add_value(4, 2 * scale_in_ms + 1);
            Assert::IsFalse(success);

            auto result = timeSeries.get_points();

            auto actual_points = result.value.get();
            compare_vectors(expected_points, *actual_points);
        }

    private:
        std::shared_ptr<ts_properties> CreateProperties()
        {
            auto properties = std::make_shared<ts_properties>();
            properties->m_metric = "test";
            return properties;
        }

        void compare_vectors(
            const std::vector<data_point>& expectedValues,
            const std::vector<data_point>& actualValues)
        {
            Assert::AreEqual(expectedValues.size(), actualValues.size());
            for (int i = 0; i < expectedValues.size(); ++i)
            {
                Assert::AreEqual(expectedValues[i].timestamp, actualValues[i].timestamp);
                Assert::AreEqual(expectedValues[i].value, actualValues[i].value);
            }
        }

        uint64_t DoubleToUint64(double value)
        {
            return *((uint64_t*)&value);
        }
    };
}
