#include "stdafx.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/ClothoDB/bitutils.h"
#include "src/ClothoDB/bitstream/bitstream.h"

using namespace incolun::clothodb;

namespace ClothDBTest
{		
	TEST_CLASS(BitUtilsTests)
	{
	public:
        TEST_METHOD(ReadBit)
        {
            auto buffer = std::make_unique<uint8_t[]>(10);
            BitUtils::WriteBits32(buffer, 0, 0b10101010, 8);
            auto result0 = BitUtils::ReadBit(buffer, 0);
            auto result1 = BitUtils::ReadBit(buffer, 1);
            auto result2 = BitUtils::ReadBit(buffer, 2);
            auto result3 = BitUtils::ReadBit(buffer, 3);
            auto result4 = BitUtils::ReadBit(buffer, 4);
            auto result5 = BitUtils::ReadBit(buffer, 5);
            auto result6 = BitUtils::ReadBit(buffer, 6);
            auto result7 = BitUtils::ReadBit(buffer, 7);

            Assert::AreEqual(1, (int)result0);
            Assert::AreEqual(0, (int)result1);
            Assert::AreEqual(1, (int)result2);
            Assert::AreEqual(0, (int)result3);
            Assert::AreEqual(1, (int)result4);
            Assert::AreEqual(0, (int)result5);
            Assert::AreEqual(1, (int)result6);
            Assert::AreEqual(0, (int)result7);
        }
		TEST_METHOD(ReadWriteBit)
		{
			BitStream buffer(10);
			buffer.WriteBits32(0b10101010, 13);
			buffer.WriteBit(1);
			buffer.WriteBit(0);
			buffer.WriteBit(1);

			buffer.Seal();
			buffer.SetPosition(0);

			auto result0 = buffer.ReadBits32(13);
			auto result1 = buffer.ReadBit();
			auto result2 = buffer.ReadBit();
			auto result3 = buffer.ReadBit();

			Assert::AreEqual(0b10101010, (int)result0);
			Assert::AreEqual(1, (int)result1);
			Assert::AreEqual(0, (int)result2);
			Assert::AreEqual(1, (int)result3);
		}

		TEST_METHOD(ReadHiBits)
        {
            auto result1 = BitUtils::ReadHiBits(0b10101010, 1);
            auto result2 = BitUtils::ReadHiBits(0b10101010, 2);
            auto result3 = BitUtils::ReadHiBits(0b10101010, 3);
            auto result4 = BitUtils::ReadHiBits(0b10101010, 4);
            auto result5 = BitUtils::ReadHiBits(0b10101010, 5);
            auto result6 = BitUtils::ReadHiBits(0b10101010, 6);
            auto result7 = BitUtils::ReadHiBits(0b10101010, 7);

            Assert::AreEqual(0b1, (int)result1);
            Assert::AreEqual(0b10, (int)result2);
            Assert::AreEqual(0b101, (int)result3);
            Assert::AreEqual(0b1010, (int)result4);
            Assert::AreEqual(0b10101, (int)result5);
            Assert::AreEqual(0b101010, (int)result6);
            Assert::AreEqual(0b1010101, (int)result7);
        }
        
        TEST_METHOD(ReadLoBits)
        {
            auto result1 = BitUtils::ReadLoBits(0b01010101, 1);
            auto result2 = BitUtils::ReadLoBits(0b01010101, 2);
            auto result3 = BitUtils::ReadLoBits(0b01010101, 3);
            auto result4 = BitUtils::ReadLoBits(0b01010101, 4);
            auto result5 = BitUtils::ReadLoBits(0b01010101, 5);
            auto result6 = BitUtils::ReadLoBits(0b01010101, 6);
            auto result7 = BitUtils::ReadLoBits(0b01010101, 7);

            Assert::AreEqual(0b1, (int)result1);
            Assert::AreEqual(0b01, (int)result2);
            Assert::AreEqual(0b101, (int)result3);
            Assert::AreEqual(0b0101, (int)result4);
            Assert::AreEqual(0b10101, (int)result5);
            Assert::AreEqual(0b010101, (int)result6);
            Assert::AreEqual(0b1010101, (int)result7);
        }

        TEST_METHOD(ReadMidBits)
        {
            auto result0 = BitUtils::ReadMidBits(0b01010101, 0, 1);
            auto result1 = BitUtils::ReadMidBits(0b01010101, 1, 1);
            auto result2 = BitUtils::ReadMidBits(0b01010101, 2, 1);
            auto result3 = BitUtils::ReadMidBits(0b01010101, 3, 1);
            auto result4 = BitUtils::ReadMidBits(0b01010101, 4, 1);
            auto result5 = BitUtils::ReadMidBits(0b01010101, 5, 1);
            auto result6 = BitUtils::ReadMidBits(0b01010101, 6, 1);
            auto result7 = BitUtils::ReadMidBits(0b01010101, 7, 1);

            Assert::AreEqual(0, (int)result0);
            Assert::AreEqual(1, (int)result1);
            Assert::AreEqual(0, (int)result2);
            Assert::AreEqual(1, (int)result3);
            Assert::AreEqual(0, (int)result4);
            Assert::AreEqual(1, (int)result5);
            Assert::AreEqual(0, (int)result6);
            Assert::AreEqual(1, (int)result7);

            auto result = BitUtils::ReadMidBits(0b01010101, 1, 6);
            Assert::AreEqual(0b101010, (int)result);

            result = BitUtils::ReadMidBits(0b01010101, 3, 3);
            Assert::AreEqual(0b101, (int)result);
            
            result = BitUtils::ReadMidBits(0b01010101, 4, 4);
            Assert::AreEqual(0b101, (int)result);
        }

        TEST_METHOD(WriteOneBitSingle)
		{
            auto buffer = std::make_unique<uint8_t[]>(10);
            uint32_t storedBits = 0;

            BitUtils::WriteBits32(buffer, storedBits, 0b101, 1);

            Assert::AreEqual((uint8_t)0b10000000, buffer[0]);
        }
        TEST_METHOD(WriteOneBitMultiple)
        {
            auto buffer = std::make_unique<uint8_t[]>(10);
            uint32_t storedBits = 0;

            for (int i = 0; i < 9; ++i)
            {
                BitUtils::WriteBits32(buffer, storedBits++, 1, 1);
            }

            Assert::AreEqual((uint8_t)0b11111111, buffer[0]);
            Assert::AreEqual((uint8_t)0b10000000, buffer[1]);
        }
        TEST_METHOD(WriteTwoBitWithOwerflow)
        {
            auto buffer = std::make_unique<uint8_t[]>(10);
            uint32_t storedBits = 7;

            BitUtils::WriteBits32(buffer, storedBits, 0b11, 2);

            Assert::AreEqual((uint8_t)0b00000001, buffer[0]);
            Assert::AreEqual((uint8_t)0b10000000, buffer[1]);
        }
        TEST_METHOD(WriteFiveBitsMultiple)
		{
            auto buffer = std::make_unique<uint8_t[]>(10);
            uint32_t storedBits = 0;

            BitUtils::WriteBits32(buffer, storedBits, 0b10001, 5); storedBits += 5;//17
            BitUtils::WriteBits32(buffer, storedBits, 0b10010, 5); storedBits += 5;//18
            BitUtils::WriteBits32(buffer, storedBits, 0b10011, 5); storedBits += 5;//19
            BitUtils::WriteBits32(buffer, storedBits, 0b10100, 5); storedBits += 5;//20

            Assert::AreEqual((uint8_t)((0b10001 << 3) + 0b100), buffer[0]);
            Assert::AreEqual((uint8_t)((0b10 << 6) + (0b10011 << 1) + 1), buffer[1]);
            Assert::AreEqual((uint8_t)(0b0100 << 4), buffer[2]);
        }

        TEST_METHOD(WriteSixtlyFourBitsSingleWithOverflow)
        {
            auto buffer = std::make_unique<uint8_t[]>(10);
            uint32_t storedBits = 1;
            uint64_t value = UINT64_MAX;

            BitUtils::WriteBits64(buffer, storedBits, value, sizeof(uint64_t) * 8);

            Assert::AreEqual((uint8_t)0x7f, buffer[0]);
            for(int i = 1; i<8;++i) 
                Assert::AreEqual((uint8_t)0xff, buffer[i]);
            Assert::AreEqual((uint8_t)0x80, buffer[8]);
        }

        TEST_METHOD(WriteDeadBeef)
        {
            BitStream buffer(10);

            buffer.WriteBits32(0xD, 4);
            buffer.WriteBits32(0xE, 4);
            buffer.WriteBits32(0xA, 4);
            buffer.WriteBits32(0xD, 4);
            buffer.WriteBits32(0xB, 4);
            buffer.WriteBits32(0xE, 4);
            buffer.WriteBits32(0xE, 4);
            buffer.WriteBits32(0xF, 4);
            
            Assert::AreEqual((uint8_t)0xDE, buffer[0]);
            Assert::AreEqual((uint8_t)0xAD, buffer[1]);
            Assert::AreEqual((uint8_t)0xBE, buffer[2]);
            Assert::AreEqual((uint8_t)0xEF, buffer[3]);
        }
        
        TEST_METHOD(ReadOneBit)
        {
            BitStream buffer(10);
            buffer.WriteBits32(0b10100101, 8);

            uint32_t result1 = buffer.ReadBits32(0, 1);
            uint32_t result2 = buffer.ReadBits32(1, 1);
            uint32_t result3 = buffer.ReadBits32(2, 1);
            uint32_t result4 = buffer.ReadBits32(3, 1);
            uint64_t result5 = buffer.ReadBits64(4, 1);
            uint64_t result6 = buffer.ReadBits64(5, 1);
            uint64_t result7 = buffer.ReadBits64(6, 1);
            uint64_t result8 = buffer.ReadBits64(7, 1);

            Assert::AreEqual(1, (int)result1);
            Assert::AreEqual(0, (int)result2);
            Assert::AreEqual(1, (int)result3);
            Assert::AreEqual(0, (int)result4);
            Assert::AreEqual(0, (int)result5);
            Assert::AreEqual(1, (int)result6);
            Assert::AreEqual(0, (int)result7);
            Assert::AreEqual(1, (int)result8);
        }

        TEST_METHOD(ReadMultipleBits)
        {
            BitStream buffer(10);

            buffer.WriteBits32(0b101, 3);
            buffer.WriteBits64(0b0101, 4);
            buffer.WriteBits32(0b00101, 5);

            uint64_t result1 = buffer.ReadBits64(3, 4);
            uint32_t result2 = buffer.ReadBits32(0, 3);
            uint64_t result3 = buffer.ReadBits64(7, 5);

            Assert::AreEqual(0b0101, (int)result1);
            Assert::AreEqual(0b101, (int)result2);
            Assert::AreEqual(0b00101, (int)result3);
        }
        
        TEST_METHOD(Read32BitsZero)
        {
            BitStream buffer(20);

            buffer.WriteBits32(0b111, 3);
            for(int i=0;i<32;++i)
                buffer.WriteBits32(0, 1);
            buffer.WriteBits32(0b11111, 5);

            uint32_t result = buffer.ReadBits32(3, 32);

            Assert::AreEqual(0, (int)result);
        }
        
        TEST_METHOD(Read64BitsZero)
        {
            BitStream buffer(20);

            buffer.WriteBits64(0b111, 3);
            for (int i = 0; i<64; ++i)
                buffer.WriteBits64(0, 1);
            buffer.WriteBits64(0b11111, 5);

            Assert::AreEqual(72, (int)buffer.GetPosition());

            uint32_t result32 = buffer.ReadBits32(3, 32);
            uint64_t result64 = buffer.ReadBits64(3, 64);

            Assert::AreEqual(0, (int)result32);
            Assert::AreEqual(0, (int)result64);
        }

        TEST_METHOD(Read32BitsDeadbeaf)
        {
            BitStream buffer(20);

            uint32_t value = 0xDEADBEEF;
            
            buffer.WriteBits32(0b111, 3);
            buffer.WriteBits32(value, 32);
            buffer.WriteBits32(0b11111, 5);
            Assert::AreEqual(40, (int)buffer.GetPosition());

            uint8_t result1 = buffer.ReadBits32(3, 8);
            uint32_t result = buffer.ReadBits32(3, 32);

            Assert::AreEqual(value, result);
        }
        
        TEST_METHOD(Read64BitsDeadbeef)
        {
            BitStream buffer(20);

            uint64_t value = 0xDEADBEEFBAAAAAAD;

            buffer.WriteBits32(0b111, 3);
            buffer.WriteBits64(value, 64);
            buffer.WriteBits32(0b11111, 5);
            Assert::AreEqual(72, (int)buffer.GetPosition());

            uint8_t result1 = buffer.ReadBits32(3, 8);
            uint64_t result = buffer.ReadBits64(3, 64);

            Assert::AreEqual(value, result);
        }

        TEST_METHOD(TestLeadingZeros32)
        {
            int result = BitUtils::clz32(1);
            Assert::AreEqual(31, result);

            result = BitUtils::clz32(0xFF);
            Assert::AreEqual(24, result);

            result = BitUtils::clz32(0xFFFF);
            Assert::AreEqual(16, result);

            result = BitUtils::clz32(0b10101010101010);
            Assert::AreEqual(18, result);

            result = BitUtils::clz32(0);
            Assert::AreEqual(32, result);

            result = BitUtils::clz32(0xFFFFFFFF);
            Assert::AreEqual(0, result);
        }

        TEST_METHOD(TestLeadingZeros64)
        {
            int result = BitUtils::clz64(1);
            Assert::AreEqual(63, result);

            result = BitUtils::clz64(0xFF);
            Assert::AreEqual(56, result);

            result = BitUtils::clz64(0xFFFF);
            Assert::AreEqual(48, result);

            result = BitUtils::clz64(0b10101010101010);
            Assert::AreEqual(50, result);

            result = BitUtils::clz64(0);
            Assert::AreEqual(64, result);

            result = BitUtils::clz64(0xFFFFFFFFFFFFFFFF);
            Assert::AreEqual(0, result);
        }

        TEST_METHOD(TestTrailingZeros32)
        {
            int result = BitUtils::ctz32(1);
            Assert::AreEqual(0, result);

            result = BitUtils::ctz32(0b10);
            Assert::AreEqual(1, result);

            result = BitUtils::ctz32(0b1000);
            Assert::AreEqual(3, result);

            result = BitUtils::ctz32(0b10101010101000);
            Assert::AreEqual(3, result);

            result = BitUtils::ctz32(0);
            Assert::AreEqual(32, result);

            result = BitUtils::ctz32(0xFFFFFFFF);
            Assert::AreEqual(0, result);
            
            result = BitUtils::ctz32(1 << 31);
            Assert::AreEqual(31, result);
        }

        TEST_METHOD(TestTrailingZeros64)
        {
            int result = BitUtils::ctz64(1);
            Assert::AreEqual(0, result);

            result = BitUtils::ctz64(0b10);
            Assert::AreEqual(1, result);

            result = BitUtils::ctz64(0b1000);
            Assert::AreEqual(3, result);

            result = BitUtils::ctz64(0b10101010101000);
            Assert::AreEqual(3, result);

            result = BitUtils::ctz64(0);
            Assert::AreEqual(64, result);

            result = BitUtils::ctz64(0xF000000100000000);
            Assert::AreEqual(32, result);
            
            result = BitUtils::ctz64(0xF000000FFFFFFFFF);
            Assert::AreEqual(0, result);

            result = BitUtils::ctz64(1 << 31);
            Assert::AreEqual(31, result);
            
            result = BitUtils::ctz64((uint64_t)1 << 63);
            Assert::AreEqual(63, result);
        }
    };
}