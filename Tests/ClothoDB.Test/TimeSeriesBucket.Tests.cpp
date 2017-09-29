#include "stdafx.h"

#include <functional>
#include <vector>
#include <limits>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/core/ts_bucket.h"
#include "src/core/time_helpers.h"

using namespace cdb::core;

namespace ClothDBTest
{
    using ts_points = std::vector<ts_point>;

    TEST_CLASS(time_series_bucket_tests)
    {
    public:
        TEST_METHOD(time_series_bucket_tests_simple)
        {
            auto properties = get_properties();
            ts_bucket bucket(properties);
            ts_points actual_points;
            ts_points expected_points{ {1, 1000}, {2, 2000} };
            for (auto &point : expected_points)
            {
                bucket.add_value(point.value, (uint32_t) point.timestamp / 1000, 0);
            }
            bucket.decompress(actual_points, 0,0, std::numeric_limits<uint32_t>::max());
            
            compare_vectors(expected_points, actual_points);
        }

        TEST_METHOD(time_series_bucket_tests_empty)
        {
            auto properties = get_properties();
            ts_bucket bucket(properties);
            ts_points actual_points;
            bucket.decompress(actual_points, 0, 0, std::numeric_limits<uint32_t>::max());

            Assert::IsTrue(actual_points.size() == 0);
        }

        TEST_METHOD(time_series_bucket_tests_single_integer)
        {
            auto properties = get_properties();
            ts_bucket bucket(properties);
            ts_points actual_points;
            ts_points expected_points{ { 1,1000 }};
            for (auto &point : expected_points)
            {
                bucket.add_value(point.value, (uint32_t)point.timestamp / 1000, 0);
            }
            bucket.decompress(actual_points, 0, 0, std::numeric_limits<uint32_t>::max());

            compare_vectors(expected_points, actual_points);
        }

        TEST_METHOD(time_series_bucket_tests_precise_time)
        {
            auto properties = get_properties();
            properties.m_store_milliseconds = true;
            ts_bucket bucket(properties);
            ts_points actual_points;
            ts_points expected_points{ { 1,1001 },{ 2,2002 } };
            for (auto &point : expected_points)
            {
                bucket.add_value(point.value, (uint32_t)point.timestamp / 1000, point.timestamp % 1000);
            }
            bucket.decompress(actual_points, 0, 0, std::numeric_limits<uint32_t>::max());

            compare_vectors(expected_points, actual_points);
        }

        TEST_METHOD(time_series_bucket_tests_single_double)
        {
            auto properties = get_properties();
            properties.m_store_milliseconds = true;
            properties.m_type = ts_type::TypeDouble;
            ts_bucket bucket(properties);
            ts_points actual_points;
            ts_points expected_points{ { DoubleToUint64(1.1), 1001 }};
            for (auto &point : expected_points)
            {
                bucket.add_value(point.value, (uint32_t)point.timestamp / 1000, point.timestamp % 1000);
            }

            bucket.decompress(actual_points, 0, 0, std::numeric_limits<uint32_t>::max());

            compare_vectors(expected_points, actual_points);
        }

        TEST_METHOD(time_series_bucket_tests_double)
        {
            auto properties = get_properties();
            properties.m_store_milliseconds = true;
            properties.m_type = ts_type::TypeDouble;
            ts_bucket bucket(properties);
            ts_points actual_points;
            ts_points expected_points
            { 
                { DoubleToUint64(1.1), 1001 },
                { DoubleToUint64(2.2), 2002 },
                { DoubleToUint64(32.32), 202002, }
            };
            for (auto &point : expected_points)
            {
                bucket.add_value(point.value, (uint32_t)point.timestamp / 1000, point.timestamp % 1000);
            }

            bucket.decompress(actual_points, 0, 0, std::numeric_limits<uint32_t>::max());

            compare_vectors(expected_points, actual_points);
        }

    private:
        ts_properties get_properties()
        {
            ts_properties properties;
            properties.m_metric = "test";
            return properties;
        }

        void compare_vectors(const ts_points& expectedValues, const ts_points& actualValues)
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