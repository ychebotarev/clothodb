#include "stdafx.h"
#include <random>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/TimeSeries/bitutils.h"
#include "src/TimeSeries/bitstream.h"
#include "src/TimeSeries/compressor/compressor.h"
#include "src/TimeSeries/compressor/decompressor.h"

using namespace incolun::clothodb;

namespace ClothDBTest
{		
	TEST_CLASS(CompressionTests)
	{
	public:
        TEST_METHOD(SingleTimestamp)
        {
			std::vector<uint32_t> expectedTimestamps;
			expectedTimestamps.push_back(0x1EEF);

			auto stream = Compressor::CompressTimestamps(expectedTimestamps);
			auto actualTimestamps = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
			CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }
		
        TEST_METHOD(SingleDoubleValue1)
        {
            std::vector<double> expectedValues;
            uint64_t expectedValue = 0xDEADBEEF;
            expectedValues.push_back(*(double*)&expectedValue);

            auto stream = Compressor::CompressDoubleValues(expectedValues);
            auto actualValues = Decompressor::DecompressDoubleValues(*stream.get());
            
            Assert::IsFalse(actualValues.IsError());
            CompareVectors(expectedValues, *actualValues.value.get());

            uint64_t actualValue = *(uint64_t*)&actualValues.value->at(0);
            Assert::AreEqual(expectedValue, actualValue);
        }
        
        TEST_METHOD(SingleDoubleValue2)
        {
            std::vector<double> expectedValues;
            double expectedValue = 12.0; 
            expectedValues.push_back(expectedValue);

            auto stream = Compressor::CompressDoubleValues(expectedValues);
            auto actualValues = Decompressor::DecompressDoubleValues(*stream.get());
            
            Assert::IsFalse(actualValues.IsError());
            CompareVectors(expectedValues, *actualValues.value.get());
        }

        TEST_METHOD(SingleIntegerValue1)
        {
            std::vector<uint64_t> expectedValues;
            uint64_t expectedValue = 0xDEADBEEF;
            expectedValues.push_back(expectedValue);

            auto stream = Compressor::CompressIntegerValues(expectedValues);
            auto actualValues = Decompressor::DecompressIntegerValues(*stream.get());
            
            Assert::IsFalse(actualValues.IsError());
            CompareVectors(expectedValues, *actualValues.value.get());

            uint64_t actualValue = actualValues.value->at(0);
            Assert::AreEqual(expectedValue, actualValue);
        }

        TEST_METHOD(SingleIntegerValue2)
        {
            std::vector<uint64_t> expectedValues;
            uint64_t expectedValue = 0xFFFFDEADBEEFFFFF;
            expectedValues.push_back(expectedValue);

            auto stream = Compressor::CompressIntegerValues(expectedValues);
            auto actualValues = Decompressor::DecompressIntegerValues(*stream.get());
            
            Assert::IsFalse(actualValues.IsError());
            CompareVectors(expectedValues, *actualValues.value.get());

            uint64_t actualValue = actualValues.value->at(0);
            Assert::AreEqual(expectedValue, actualValue);
        }

        TEST_METHOD(MultipleDoubleValuesSimple)
        {
            std::vector<double> expectedValues;
            double f1 = 21.0 / 10;
            expectedValues.push_back(21.0 / 10);
            expectedValues.push_back(21.016 / 10);
            expectedValues.push_back(25.0 / 10);
            expectedValues.push_back(31.0 / 10);
            expectedValues.push_back(28.2 / 10);

            auto stream = Compressor::CompressDoubleValues(expectedValues);
            auto actualValues = Decompressor::DecompressDoubleValues(*stream.get());
            
            Assert::IsFalse(actualValues.IsError());
            CompareVectors(expectedValues, *actualValues.value.get());
        }

        TEST_METHOD(MultipleTimestampValuesSimple)
        {
            std::vector<uint32_t> expectedValues;
            expectedValues.push_back(21);
            expectedValues.push_back(21);
            expectedValues.push_back(25);
            expectedValues.push_back(31);
            expectedValues.push_back(28);
            expectedValues.push_back(27);

            auto stream = Compressor::CompressTimestamps(expectedValues);
            auto actualValues = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualValues.IsError());
            CompareVectors(expectedValues, *actualValues.value.get());
        }

        TEST_METHOD(MultipleIntegerValuesSimple)
        {
            std::vector<uint64_t> expectedValues;
            expectedValues.push_back(21);
            expectedValues.push_back(21);
            expectedValues.push_back(25);
            expectedValues.push_back(31);
            expectedValues.push_back(28);
            expectedValues.push_back(27);

            auto stream = Compressor::CompressIntegerValues(expectedValues);
            auto actualValues = Decompressor::DecompressIntegerValues(*stream.get());
            
            Assert::IsFalse(actualValues.IsError());
            CompareVectors(expectedValues, *actualValues.value.get());
        }

        TEST_METHOD(IncreasingTimestampSequenceSimple)
		{
			std::vector<uint32_t> expectedTimestamps;
            expectedTimestamps.push_back(1);
            expectedTimestamps.push_back(2);
            expectedTimestamps.push_back(3);

			auto stream = Compressor::CompressTimestamps(expectedTimestamps);
			auto actualTimestamps = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
			CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
		}
        
        TEST_METHOD(IncreasingIntegerSequenceSimple)
        {
            std::vector<uint64_t> expectedTimestamps;
            expectedTimestamps.push_back(1);
            expectedTimestamps.push_back(2);
            expectedTimestamps.push_back(3);
            expectedTimestamps.push_back((uint64_t)0xFFFFFFFF + 1);
            expectedTimestamps.push_back((uint64_t)0xFFFFFFFF + 2);
            expectedTimestamps.push_back((uint64_t)0xFFFFFFFF + 3);

            auto stream = Compressor::CompressIntegerValues(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressIntegerValues(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

        TEST_METHOD(IncreasingTimestampSequence)
        {
            std::vector<uint32_t> expectedTimestamps;
            expectedTimestamps.push_back(1);
            for (int i = 13; i >= 1; --i)
            {
                expectedTimestamps.push_back(3600/i);
            }

            auto stream = Compressor::CompressTimestamps(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

        TEST_METHOD(IncreasingIntegerSequence)
        {
            std::vector<uint64_t> expectedTimestamps;
            expectedTimestamps.push_back(1);
            for (int i = 2; i <= 64; ++i)
            {
                expectedTimestamps.push_back((uint64_t)1 << i);
            }

            auto stream = Compressor::CompressIntegerValues(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressIntegerValues(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

        TEST_METHOD(DecreasingTimestampSequenceSimple)
        {
            std::vector<uint32_t> expectedTimestamps;
            expectedTimestamps.push_back(3);
            expectedTimestamps.push_back(2);
            expectedTimestamps.push_back(1);

            auto stream = Compressor::CompressTimestamps(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

        TEST_METHOD(DecreasingTimestampSequence)
        {
            std::vector<uint32_t> expectedTimestamps;
            for (int i = 1; i <= 13; ++i)
            {
                expectedTimestamps.push_back(3600 / i);
            }

            auto stream = Compressor::CompressTimestamps(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

        TEST_METHOD(DecreasingIntegerSequence)
        {
            std::vector<uint64_t> expectedTimestamps;
            for (int i = 63; i >= 0; --i)
            {
                expectedTimestamps.push_back((uint64_t)1 << i);
            }

            auto stream = Compressor::CompressIntegerValues(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressIntegerValues(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

        TEST_METHOD(MixedTimestampSequenceSimple)
        {
            std::vector<uint32_t> expectedTimestamps;
            expectedTimestamps.push_back(10);
            expectedTimestamps.push_back(11);
            expectedTimestamps.push_back(18);
            expectedTimestamps.push_back(19);
            expectedTimestamps.push_back(100);

            auto stream = Compressor::CompressTimestamps(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

        TEST_METHOD(MixedTimestampSequenceRandom)
        {
            std::vector<uint32_t> expectedTimestamps;
            expectedTimestamps.push_back(52);
            expectedTimestamps.push_back(22);
            expectedTimestamps.push_back(21);
            expectedTimestamps.push_back(116);

            for (int i = 0; i < 30; ++i)
            {
                expectedTimestamps.push_back( rand() % 120 );
            }

            auto stream = Compressor::CompressTimestamps(expectedTimestamps);
            auto actualTimestamps = Decompressor::DecompressTimeStamps(*stream.get());
            
            Assert::IsFalse(actualTimestamps.IsError());
            CompareVectors(expectedTimestamps, *actualTimestamps.value.get());
        }

    private:
		template <typename T>
		void CompareVectors(const std::vector<T>& expectedValues, const std::vector<T>& actualValues)
		{
			Assert::AreEqual(expectedValues.size(), actualValues.size());
			for (int i = 0; i < expectedValues.size(); ++i)
			{
				Assert::AreEqual(expectedValues[i], actualValues[i]);
			}
		}
    };
}