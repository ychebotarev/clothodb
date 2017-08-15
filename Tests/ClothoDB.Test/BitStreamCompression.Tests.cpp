#include "stdafx.h"
#include <random>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/ClothoDB/bitutils.h"
#include "src/ClothoDB/bitstream/bitstream.h"
#include "src/ClothoDB/bitstream/bitstreamcompressor.h"
#include "src/ClothoDB/bitstream/bitstreamdecompressor.h"

using namespace incolun::clothodb;

namespace ClothDBTest
{		
	TEST_CLASS(BitStreamCompressionTests)
	{
	public:
        TEST_METHOD(SingleTimestamp)
        {
			std::vector<uint32_t> expectedTimestamps;
			expectedTimestamps.push_back(0x1EEF);

			auto bitStream = BitStreamCompressor::CompressTimestamps(expectedTimestamps);
			bitStream->Seal();

			auto actualTimestamps = BitStreamDecompressor::DecompressTimeStamps(bitStream);

			CompareVectors(expectedTimestamps, actualTimestamps);
        }
		
        TEST_METHOD(SingleIntegerValue)
        {
            std::vector<double> expectedValues;
            uint64_t expectedValue = 0xDEADBEEF;
            expectedValues.push_back(*(double*)&expectedValue);

            auto bitStream = BitStreamCompressor::CompressValues(expectedValues);
            bitStream->Seal();

            auto actualValues = BitStreamDecompressor::DecompressValues(bitStream);

            CompareVectors(expectedValues, actualValues);
            uint64_t actualValue = *(uint64_t*)&actualValues[0];
            Assert::AreEqual(expectedValue, actualValue);
        }
        
        TEST_METHOD(SingleDoubleValue)
        {
            std::vector<double> expectedValues;
            double expectedValue = 12.0; 
            expectedValues.push_back(expectedValue);

            auto bitStream = BitStreamCompressor::CompressValues(expectedValues);
            bitStream->Seal();

            auto actualValues = BitStreamDecompressor::DecompressValues(bitStream);

            CompareVectors(expectedValues, actualValues);
        }

        TEST_METHOD(MultipleDoubleValuesSimple)
        {
            std::vector<double> expectedValues;
            float f1 = 21.0 / 10;
            expectedValues.push_back(21.0 / 10);
            expectedValues.push_back(21.016 / 10);
            expectedValues.push_back(25.0 / 10);
            expectedValues.push_back(31.0 / 10);
            expectedValues.push_back(28.2 / 10);

            auto bitStream = BitStreamCompressor::CompressValues(expectedValues);
            bitStream->Seal();

            auto actualValues = BitStreamDecompressor::DecompressValues(bitStream);

            CompareVectors(expectedValues, actualValues);
        }

        TEST_METHOD(IncreasingTimestampSequenceSimple)
		{
			std::vector<uint32_t> expectedTimestamps;
            expectedTimestamps.push_back(1);
            expectedTimestamps.push_back(2);

			auto bitStream = BitStreamCompressor::CompressTimestamps(expectedTimestamps);
			bitStream->Seal();

			auto actualTimestamps = BitStreamDecompressor::DecompressTimeStamps(bitStream);

			CompareVectors(expectedTimestamps, actualTimestamps);
		}
        
        TEST_METHOD(MixedTimestampSequenceSimple)
        {
            std::vector<uint32_t> expectedTimestamps;
            expectedTimestamps.push_back(10);
            expectedTimestamps.push_back(11);
            expectedTimestamps.push_back(18);
            expectedTimestamps.push_back(19);
            expectedTimestamps.push_back(100);

            auto bitStream = BitStreamCompressor::CompressTimestamps(expectedTimestamps);
            bitStream->Seal();

            auto actualTimestamps = BitStreamDecompressor::DecompressTimeStamps(bitStream);

            CompareVectors(expectedTimestamps, actualTimestamps);
        }

        TEST_METHOD(MixedTimestampSequenceRandom)
        {
            std::vector<uint32_t> expectedTimestamps;
            uint32_t timestamp = 100;
            expectedTimestamps.push_back(timestamp);

            for (int i = 0; i < 1000; ++i)
            {
                expectedTimestamps.push_back( rand() % 120 );
            }

            auto bitStream = BitStreamCompressor::CompressTimestamps(expectedTimestamps);
            bitStream->Seal();

            auto actualTimestamps = BitStreamDecompressor::DecompressTimeStamps(bitStream);

            CompareVectors(expectedTimestamps, actualTimestamps);
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