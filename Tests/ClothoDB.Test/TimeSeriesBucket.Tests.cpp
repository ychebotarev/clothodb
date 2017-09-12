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
            bucket.AddValue(1, 1000);
            bucket.AddValue(2, 2000);

            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints{ {1000,1}, {2000,2} };
            bucket.Decompress(actualPoints, 0,0, std::numeric_limits<uint32_t>::max());
            
            CompareVectors(expectedPoints, actualPoints);
        }

        TEST_METHOD(TimeSeriesBucketTestsMilliseconds)
        {
            auto config = GetSimpleConfig();
            config.m_storeMilliseconds = true;
            TimeSeriesBucket bucket(config);
            bucket.AddValue(1, 1001);
            bucket.AddValue(2, 2002);

            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints{ { 1001,1 },{ 2002,2 } };
            bucket.Decompress(actualPoints, 0, 0, std::numeric_limits<uint32_t>::max());

            CompareVectors(expectedPoints, actualPoints);
        }

        TEST_METHOD(TimeSeriesBucketTestsDouble)
        {
            auto config = GetSimpleConfig();
            config.m_storeMilliseconds = true;
            config.m_type = TimeSeriesType::TypeDouble;
            TimeSeriesBucket bucket(config);
            bucket.AddValue(1.1, 1001);
            bucket.AddValue(2.2, 2002);

            TimeSeriesPoints actualPoints;
            TimeSeriesPoints expectedPoints{ { 1001,1.1 },{ 2002,2.2 } };
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
    };
}