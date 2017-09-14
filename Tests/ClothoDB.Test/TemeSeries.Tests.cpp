#include "stdafx.h"

#include <functional>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/TimeSeries/constants.h"
#include "src/TimeSeries/TimeSeries.h"

using namespace incolun::clothodb;

namespace ClothDBTest
{
    TEST_CLASS(TimeSeriesTests)
    {
    public:
        TEST_METHOD(TimeSeriesTestSimple)
        {
            auto config = GetSimpleConfig();
            TimeSeries timeSeries(config);
            std::vector<TimeSeriesPoint> expectedPoints{ { 1, 1000 },{ 2, 2000 } };
            for (auto &point : expectedPoints)
            {
                timeSeries.AddValue(point.value, point.timestamp);
            }

            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }

        TEST_METHOD(TimeSeriesTestEmpty)
        {
            auto config = GetSimpleConfig();
            TimeSeries timeSeries(config);

            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            Assert::IsTrue(actualPoints->size() == 0);
        }

        TEST_METHOD(TimeSeriesTestSimpleInteger)
        {
            auto config = GetSimpleConfig();
            TimeSeries timeSeries(config);
            std::vector<TimeSeriesPoint> expectedPoints{ { 0, 1000 } };
            for (auto &point : expectedPoints)
            {
                timeSeries.AddValue(point.value, point.timestamp);
            }
            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }
        
        TEST_METHOD(TimeSeriesTestSimpleDouble)
        {
            auto config = GetSimpleConfig();
            config->m_type = TimeSeriesType::TypeDouble;
            TimeSeries timeSeries(config);
            std::vector<TimeSeriesPoint> expectedPoints{ { 0, 1000 } };
            for (auto &point : expectedPoints)
            {
                timeSeries.AddValue(point.value, point.timestamp);
            }
            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }
        
        TEST_METHOD(TimeSeriesTestSimpleMilliseconds)
        {
            auto config = GetSimpleConfig();
            config->m_type = TimeSeriesType::TypeDouble;
            config->m_storeMilliseconds = true;
            TimeSeries timeSeries(config);
            std::vector<TimeSeriesPoint> expectedPoints{ { 0, 1000 } };
            for (auto &point : expectedPoints)
            {
                timeSeries.AddValue(point.value, point.timestamp);
            }

            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }

        TEST_METHOD(TimeSeriesTestSimpleNoMilliseconds)
        {
            auto config = GetSimpleConfig();
            config->m_type = TimeSeriesType::TypeDouble;
            TimeSeries timeSeries(config);
            std::vector<TimeSeriesPoint> expectedPoints{ { 0, 1000 } };
            for (auto &point : expectedPoints)
            {
                timeSeries.AddValue(point.value, point.timestamp);
            }
            
            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }

        TEST_METHOD(TimeSeriesTestMultipleBucketsSimple)
        {
            auto config = GetSimpleConfig();
            config->m_type = TimeSeriesType::TypeInteger;
            TimeSeries timeSeries(config);
            uint64_t startTime = 1000;
            std::vector<TimeSeriesPoint> expectedPoints;
            for (int hour = 0; hour < 2; ++hour)
            {
                for (int minute = 0; minute < 60; ++minute)
                {
                    uint64_t totalMintes = hour * 60 + minute;
                    uint64_t timestamp = startTime + totalMintes * 60 * 1000;
                    expectedPoints.push_back({ totalMintes, timestamp });
                }
            }
            for (auto &point : expectedPoints)
            {
                timeSeries.AddValue(point.value, point.timestamp);
            }

            Assert::AreEqual(2, (int)timeSeries.StoredHours());
            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }

        TEST_METHOD(TimeSeriesTestMultipleBucketsOverflaw)
        {
            auto config = GetSimpleConfig();
            config->m_type = TimeSeriesType::TypeInteger;
            TimeSeries timeSeries(config);
            uint64_t startTime = 1000;

            std::vector<TimeSeriesPoint> expectedPoints;
            for (int hour = 0; hour <= Constants::kMaxBuckets; ++hour)
            {
                for (int minute = 0; minute < 60; ++minute)
                {
                    uint64_t totalMintes = hour * 60 + minute;
                    uint64_t timestamp = startTime + totalMintes * 60 * 1000;
                    timeSeries.AddValue(totalMintes, timestamp);
                    if(hour >= 1)
                        expectedPoints.push_back({ totalMintes, timestamp });
                }
            }
            auto storedHours = timeSeries.StoredHours();
            Assert::AreEqual(Constants::kMaxBuckets, storedHours);
            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);
            
            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }
        
        TEST_METHOD(TimeSeriesTestRemoveOldData)
        {
            auto config = GetSimpleConfig();
            config->m_type = TimeSeriesType::TypeInteger;
            TimeSeries timeSeries(config);
            uint64_t startTime = 1000;

            uint64_t oldTime = 0;
            std::vector<TimeSeriesPoint> expectedPoints;
            for (int hour = 0; hour <= Constants::kMaxBuckets; ++hour)
            {
                if (hour < 3)
                {
                    oldTime = hour * Constants::kOneHourInMs;
                }
                for (int minute = 0; minute < 60; ++minute)
                {
                    uint64_t totalMintes = hour * 60 + minute;
                    uint64_t timestamp = startTime + totalMintes * 60 * 1000;
                    timeSeries.AddValue(totalMintes, timestamp);
                    if (hour >= 3)
                        expectedPoints.push_back({ totalMintes, timestamp });
                }
            }
            oldTime += 60 * 1000;
            timeSeries.RemoveOldData(oldTime);
            auto storedHours = timeSeries.StoredHours();
            Assert::AreEqual(Constants::kMaxBuckets - 2, storedHours);
            auto result = timeSeries.GetPoints(0, 0xffffffffffffffff);

            auto actualPoints = result.value.get();
            CompareVectors(expectedPoints, *actualPoints);
        }
        

    private:
        std::shared_ptr<TimeSeriesConfig> GetSimpleConfig()
        {
            auto config = std::make_shared<TimeSeriesConfig>();
            config->m_metricName = "test";
            return config;
        }

        void CompareVectors(
            const std::vector<TimeSeriesPoint>& expectedValues, 
            const std::vector<TimeSeriesPoint>& actualValues)
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
