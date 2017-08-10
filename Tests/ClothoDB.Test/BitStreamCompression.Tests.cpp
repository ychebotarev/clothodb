#include "stdafx.h"
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
			expectedTimestamps.push_back(0xBEEF);

			auto bitStream = BitStreamCompressor::CompressTimestamps(expectedTimestamps);
			bitStream->Seal();

			auto actualTimestamps = BitStreamDecompressor::DecompressTimeStampts(bitStream);

			CompareTimestamps(expectedTimestamps, actualTimestamps);
        }
		
		TEST_METHOD(IncreasingSequence)
		{
			std::vector<uint32_t> expectedTimestamps;
			for(int i=1; i< 100;++i) expectedTimestamps.push_back(i);

			auto bitStream = BitStreamCompressor::CompressTimestamps(expectedTimestamps);
			bitStream->Seal();

			auto actualTimestamps = BitStreamDecompressor::DecompressTimeStampts(bitStream);

			CompareTimestamps(expectedTimestamps, actualTimestamps);
		}
	private:
		template <typename T>
		void CompareTimestamps(const std::vector<T>& expectedTimestamps, const std::vector<T>& actualTimestamps)
		{
			Assert::AreEqual(expectedTimestamps.size(), actualTimestamps.size());
			for (int i = 0; i < expectedTimestamps.size(); ++i)
			{
				Assert::AreEqual(expectedTimestamps[i], actualTimestamps[i]);
			}
		}
    };
}