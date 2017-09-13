#include "stdafx.h"

#include <functional>
#include <vector>
#include <limits>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/TimeSeries/TimeSeriesBucket.h"

using namespace incolun::clothodb;

namespace ClothDBTest
{
    using TimeSeriesPoints = std::vector<TimeSeriesPoint>;

    TEST_CLASS(TimeSeriesBucketTests)
    {
    public:
        TEST_METHOD(TimeSeriesBucketTestsSimple)
        {
            auto config = GetSimpleConfig();
            TimeSeriesBucket bucket(config);
            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints{ {1, 1000}, {2, 2000} };
            for (auto &point : expectedPoints)
            {
                bucket.AddValue(point.value, point.timestamp);
            }
            bucket.Decompress(actualPoints, 0,0, std::numeric_limits<uint32_t>::max());
            
            CompareVectors(expectedPoints, actualPoints);
        }

        TEST_METHOD(TimeSeriesBucketTestsEmpty)
        {
            auto config = GetSimpleConfig();
            TimeSeriesBucket bucket(config);
            TimeSeriesPoints actualPoints;
            bucket.Decompress(actualPoints, 0, 0, std::numeric_limits<uint32_t>::max());

            Assert::IsTrue(actualPoints.size() == 0);
        }

        TEST_METHOD(TimeSeriesBucketTestsSingleInteger)
        {
            auto config = GetSimpleConfig();
            TimeSeriesBucket bucket(config);
            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints{ { 1,1000 }};
            for (auto &point : expectedPoints)
            {
                bucket.AddValue(point.value, point.timestamp);
            }
            bucket.Decompress(actualPoints, 0, 0, std::numeric_limits<uint32_t>::max());

            CompareVectors(expectedPoints, actualPoints);
        }

        TEST_METHOD(TimeSeriesBucketTestsMilliseconds)
        {
            auto config = GetSimpleConfig();
            config.m_storeMilliseconds = true;
            TimeSeriesBucket bucket(config);
            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints{ { 1,1001 },{ 2,2002 } };
            for (auto &point : expectedPoints)
            {
                bucket.AddValue(point.value, point.timestamp);
            }
            bucket.Decompress(actualPoints, 0, 0, std::numeric_limits<uint32_t>::max());

            CompareVectors(expectedPoints, actualPoints);
        }

        TEST_METHOD(TimeSeriesBucketTestsSingleDouble)
        {
            auto config = GetSimpleConfig();
            config.m_storeMilliseconds = true;
            config.m_type = TimeSeriesType::TypeDouble;
            TimeSeriesBucket bucket(config);
            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints{ { DoubleToUint64(1.1), 1001 }};
            for (auto &point : expectedPoints)
            {
                bucket.AddValue(point.value, point.timestamp);
            }

            bucket.Decompress(actualPoints, 0, 0, std::numeric_limits<uint32_t>::max());

            CompareVectors(expectedPoints, actualPoints);
        }

        TEST_METHOD(TimeSeriesBucketTestsDouble)
        {
            auto config = GetSimpleConfig();
            config.m_storeMilliseconds = true;
            config.m_type = TimeSeriesType::TypeDouble;
            TimeSeriesBucket bucket(config);
            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints
            { 
                { DoubleToUint64(1.1), 1001 },
                { DoubleToUint64(2.2), 2002 },
                { DoubleToUint64(32.32), 202002, }
            };
            for (auto &point : expectedPoints)
            {
                bucket.AddValue(point.value, point.timestamp);
            }

            bucket.Decompress(actualPoints, 0, 0, std::numeric_limits<uint32_t>::max());

            CompareVectors(expectedPoints, actualPoints);
        }

    private:
        TimeSeriesConfig GetSimpleConfig()
        {
            TimeSeriesConfig config;
            config.m_metricName = "test";
            return config;
        }

        void CompareVectors(const TimeSeriesPoints& expectedValues, const TimeSeriesPoints& actualValues)
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