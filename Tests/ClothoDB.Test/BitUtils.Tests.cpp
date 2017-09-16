#include "stdafx.h"

#include <functional>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/core/bit_utils.h"
#include "src/core/bit_stream.h"

using namespace clothodb::core;

namespace ClothDBTest
{		
	TEST_CLASS(BitUtilsTests)
	{
	public:
        TEST_METHOD(BitUtilsTestsDummy)
        {
            bit_stream stream(10);
            bit_stream_writer writer(stream);
            writer.write_bits32(0b11111111111, 11);
            writer.commit();

            auto result = stream.read_bits32(0, 2);

            Assert::AreEqual(0b11, (int)result);
        }

        TEST_METHOD(TestBitStreamCapacity)
        {
            bit_stream stream(10);
            bit_stream_writer writer(stream);

            for (uint64_t i = 0; i < 100; ++i)
            {
                writer.write_bits32((uint32_t)i, 32);
                writer.write_bits64(i, 64);
            }

            writer.commit();

            bit_stream_reader reader(stream);
            for (uint64_t i = 0; i < 100; ++i)
            {
                auto result32 = reader.read_bits32(32);
                auto result64 = reader.read_bits64(64);
                Assert::AreEqual((uint32_t)i, result32);
                Assert::AreEqual(i, result64);
            }
        }
        
        TEST_METHOD(ReadBits)
        {
            {
                auto buffer = std::vector<uint8_t>(10);

                bit_utils::write_bits32(buffer, 0, 0b10101010, 8);
                bit_utils::write_bits32(buffer, 8, 1, 1);
                bit_utils::write_bits32(buffer, 9, 0, 1);

                auto result0 = bit_utils::read_bit(buffer, 0);
                auto result1 = bit_utils::read_bit(buffer, 1);
                auto result2 = bit_utils::read_bit(buffer, 2);
                auto result3 = bit_utils::read_bit(buffer, 3);
                auto result4 = bit_utils::read_bit(buffer, 4);
                auto result5 = bit_utils::read_bit(buffer, 5);
                auto result6 = bit_utils::read_bit(buffer, 6);
                auto result7 = bit_utils::read_bit(buffer, 7);
                auto result8 = bit_utils::read_bit(buffer, 8);
                auto result9 = bit_utils::read_bit(buffer, 9);

                Assert::AreEqual(1, (int)result0);
                Assert::AreEqual(0, (int)result1);
                Assert::AreEqual(1, (int)result2);
                Assert::AreEqual(0, (int)result3);
                Assert::AreEqual(1, (int)result4);
                Assert::AreEqual(0, (int)result5);
                Assert::AreEqual(1, (int)result6);
                Assert::AreEqual(0, (int)result7);

                Assert::AreEqual(1, (int)result8);
                Assert::AreEqual(0, (int)result9);
            }

            {
                bit_stream stream(10);

                stream.write_bits32(0, 0b11111111111, 11);
                stream.set_commited_bits(11);
                
                auto result = stream.read_bits32(0, 2);
                Assert::AreEqual(0b11, (int)result);
            }

            {
                bit_stream stream(10);
                stream.write_bits32(0, 0xFF00FF00, 32);
                stream.set_commited_bits(32);

                auto result1 = stream.read_bits32(13, 7);
                Assert::AreEqual(0b1111, (int)result1);
                
                auto result2 = stream.read_bits32(7, 12);
                Assert::AreEqual(0b100000000111, (int)result2);
            }
        }

		TEST_METHOD(ReadWriteBit)
		{
			bit_stream stream(10);
            bit_stream_writer writer(stream);
            bit_stream_reader reader(stream);

            writer.write_bits32(0b10101010, 13);
            writer.write_bit(1);
            writer.write_bit(0);
            writer.write_bit(1);
            writer.commit();

			auto result0 = reader.read_bits32(13);
			auto result1 = reader.read_bit();
			auto result2 = reader.read_bit();
			auto result3 = reader.read_bit();
            
            Assert::AreEqual(0b10101010, (int)result0);
            Assert::AreEqual(1, (int)result1);
            Assert::AreEqual(0, (int)result2);
            Assert::AreEqual(1, (int)result3);

            auto f1 = [&reader]() { reader.read_bit(); };

            auto checkException = [](std::function<void()> func)
            {
                bool exception = false;
                try
                {
                    func();
                }
                catch (const std::overflow_error&)
                {
                    exception = true;
                }
                Assert::IsTrue(exception);
            };

            checkException([&reader]() { reader.read_bit(); });
            checkException([&reader]() { reader.read_bits32(10); });
            checkException([&reader]() { reader.read_bits64(50); });
        }

		TEST_METHOD(read_hi_bits)
        {
            auto result1 = bit_utils::read_hi_bits(0b10101010, 1);
            auto result2 = bit_utils::read_hi_bits(0b10101010, 2);
            auto result3 = bit_utils::read_hi_bits(0b10101010, 3);
            auto result4 = bit_utils::read_hi_bits(0b10101010, 4);
            auto result5 = bit_utils::read_hi_bits(0b10101010, 5);
            auto result6 = bit_utils::read_hi_bits(0b10101010, 6);
            auto result7 = bit_utils::read_hi_bits(0b10101010, 7);

            Assert::AreEqual(0b1, (int)result1);
            Assert::AreEqual(0b10, (int)result2);
            Assert::AreEqual(0b101, (int)result3);
            Assert::AreEqual(0b1010, (int)result4);
            Assert::AreEqual(0b10101, (int)result5);
            Assert::AreEqual(0b101010, (int)result6);
            Assert::AreEqual(0b1010101, (int)result7);
        }
        
        TEST_METHOD(read_lo_bits)
        {
            auto result1 = bit_utils::read_lo_bits(0b01010101, 1);
            auto result2 = bit_utils::read_lo_bits(0b01010101, 2);
            auto result3 = bit_utils::read_lo_bits(0b01010101, 3);
            auto result4 = bit_utils::read_lo_bits(0b01010101, 4);
            auto result5 = bit_utils::read_lo_bits(0b01010101, 5);
            auto result6 = bit_utils::read_lo_bits(0b01010101, 6);
            auto result7 = bit_utils::read_lo_bits(0b01010101, 7);

            Assert::AreEqual(0b1, (int)result1);
            Assert::AreEqual(0b01, (int)result2);
            Assert::AreEqual(0b101, (int)result3);
            Assert::AreEqual(0b0101, (int)result4);
            Assert::AreEqual(0b10101, (int)result5);
            Assert::AreEqual(0b010101, (int)result6);
            Assert::AreEqual(0b1010101, (int)result7);
        }

        TEST_METHOD(read_mid_bits)
        {
            auto result0 = bit_utils::read_mid_bits(0b01010101, 0, 1);
            auto result1 = bit_utils::read_mid_bits(0b01010101, 1, 1);
            auto result2 = bit_utils::read_mid_bits(0b01010101, 2, 1);
            auto result3 = bit_utils::read_mid_bits(0b01010101, 3, 1);
            auto result4 = bit_utils::read_mid_bits(0b01010101, 4, 1);
            auto result5 = bit_utils::read_mid_bits(0b01010101, 5, 1);
            auto result6 = bit_utils::read_mid_bits(0b01010101, 6, 1);
            auto result7 = bit_utils::read_mid_bits(0b01010101, 7, 1);

            Assert::AreEqual(0, (int)result0);
            Assert::AreEqual(1, (int)result1);
            Assert::AreEqual(0, (int)result2);
            Assert::AreEqual(1, (int)result3);
            Assert::AreEqual(0, (int)result4);
            Assert::AreEqual(1, (int)result5);
            Assert::AreEqual(0, (int)result6);
            Assert::AreEqual(1, (int)result7);

            auto result = bit_utils::read_mid_bits(0b01010101, 1, 6);
            Assert::AreEqual(0b101010, (int)result);

            result = bit_utils::read_mid_bits(0b01010101, 3, 3);
            Assert::AreEqual(0b101, (int)result);
            
            result = bit_utils::read_mid_bits(0b01010101, 4, 4);
            Assert::AreEqual(0b101, (int)result);
        }

        TEST_METHOD(WriteOneBitSingle)
		{
            std::vector<uint8_t> buffer(10);
            uint32_t storedBits = 0;

            bit_utils::write_bits32(buffer, storedBits, 0b101, 1);

            Assert::AreEqual((uint8_t)0b10000000, buffer[0]);
        }
        TEST_METHOD(WriteOneBitMultiple)
        {
            std::vector<uint8_t> buffer(10);
            uint32_t storedBits = 0;

            for (int i = 0; i < 9; ++i)
            {
                bit_utils::write_bits32(buffer, storedBits++, 1, 1);
            }

            Assert::AreEqual((uint8_t)0b11111111, buffer[0]);
            Assert::AreEqual((uint8_t)0b10000000, buffer[1]);
        }

        TEST_METHOD(WriteTwoBitWithOwerflow)
        {
            std::vector<uint8_t> buffer(10);
            uint32_t storedBits = 7;

            bit_utils::write_bits32(buffer, storedBits, 0b11, 2);

            Assert::AreEqual((uint8_t)0b00000001, buffer[0]);
            Assert::AreEqual((uint8_t)0b10000000, buffer[1]);
        }

        TEST_METHOD(WriteFiveBitsMultiple)
		{
            std::vector<uint8_t> buffer(10);
            uint32_t storedBits = 0;

            bit_utils::write_bits32(buffer, storedBits, 0b10001, 5); storedBits += 5;//17
            bit_utils::write_bits32(buffer, storedBits, 0b10010, 5); storedBits += 5;//18
            bit_utils::write_bits32(buffer, storedBits, 0b10011, 5); storedBits += 5;//19
            bit_utils::write_bits32(buffer, storedBits, 0b10100, 5); storedBits += 5;//20

            Assert::AreEqual((uint8_t)((0b10001 << 3) + 0b100), buffer[0]);
            Assert::AreEqual((uint8_t)((0b10 << 6) + (0b10011 << 1) + 1), buffer[1]);
            Assert::AreEqual((uint8_t)(0b0100 << 4), buffer[2]);
        }

        TEST_METHOD(WriteSixtlyFourBitsSingleWithOverflow)
        {
            std::vector<uint8_t> buffer(10);
            uint32_t storedBits = 1;
            uint64_t value = UINT64_MAX;

            bit_utils::write_bits64(buffer, storedBits, value, sizeof(uint64_t) * 8);

            Assert::AreEqual((uint8_t)0x7f, buffer[0]);
            for(int i = 1; i<8;++i) 
                Assert::AreEqual((uint8_t)0xff, buffer[i]);
            Assert::AreEqual((uint8_t)0x80, buffer[8]);
        }

        TEST_METHOD(WriteDeadBeef)
        {
            bit_stream stream(10);
            bit_stream_writer writer(stream);

            writer.write_bits32(0xD, 4);
            writer.write_bits32(0xE, 4);
            writer.write_bits32(0xA, 4);
            writer.write_bits32(0xD, 4);
            writer.write_bits32(0xB, 4);
            writer.write_bits32(0xE, 4);
            writer.write_bits32(0xE, 4);
            writer.write_bits32(0xF, 4);
            
            Assert::AreEqual((uint8_t)0xDE, stream[0]);
            Assert::AreEqual((uint8_t)0xAD, stream[1]);
            Assert::AreEqual((uint8_t)0xBE, stream[2]);
            Assert::AreEqual((uint8_t)0xEF, stream[3]);
        }
        
        TEST_METHOD(ReadOneBit)
        {
            bit_stream stream(10);
            bit_stream_writer writer(stream);
            writer.write_bits32(0b10100101, 8);
            writer.commit();

            auto result1 = stream.read_bits32(0, 1);
            auto result2 = stream.read_bits32(1, 1);
            auto result3 = stream.read_bits32(2, 1);
            auto result4 = stream.read_bits32(3, 1);
            auto result5 = stream.read_bits64(4, 1);
            auto result6 = stream.read_bits64(5, 1);
            auto result7 = stream.read_bits64(6, 1);
            auto result8 = stream.read_bits64(7, 1);

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
            bit_stream stream(10);
            bit_stream_writer writer(stream);

            writer.write_bits32(0b101, 3);
            writer.write_bits64(0b0101, 4);
            writer.write_bits32(0b00101, 5);
            writer.commit();

            auto result1 = stream.read_bits64(3, 4);
            auto result2 = stream.read_bits32(0, 3);
            auto result3 = stream.read_bits64(7, 5);

            Assert::AreEqual(0b0101, (int)result1);
            Assert::AreEqual(0b101, (int)result2);
            Assert::AreEqual(0b00101, (int)result3);
        }
        
        TEST_METHOD(Read32BitsZero)
        {
            bit_stream stream(20);
            bit_stream_writer writer(stream);

            writer.write_bits32(0b111, 3);
            for(int i=0;i<32;++i)
                writer.write_bits32(0, 1);
            writer.write_bits32(0b11111, 5);
            writer.commit();

            auto result = stream.read_bits32(3, 32);

            Assert::AreEqual(0, (int)result);
        }
        
        TEST_METHOD(Read64BitsZero)
        {
            bit_stream stream(20);
            bit_stream_writer writer(stream);

            writer.write_bits64(0b111, 3);
            for (int i = 0; i<64; ++i)
                writer.write_bits64(0, 1);
            writer.write_bits64(0b11111, 5);
            writer.commit();

            auto result32 = stream.read_bits32(3, 32);
            auto result64 = stream.read_bits64(3, 64);

            Assert::AreEqual(72, (int)stream.get_commited_bits());
            Assert::AreEqual(0, (int)result32);
            Assert::AreEqual(0, (int)result64);
        }

        TEST_METHOD(Read32BitsDeadbeaf)
        {
            bit_stream stream(20);
            bit_stream_writer writer(stream);

            uint32_t value = 0xDEADBEEF;
            
            writer.write_bits32(0b111, 3);
            writer.write_bits32(value, 32);
            writer.write_bits32(0b11111, 5);
            writer.commit();

            auto result1 = stream.read_bits32(3, 8);
            auto result2 = stream.read_bits32(3, 32);

            Assert::AreEqual(40, (int)stream.get_commited_bits());
            Assert::AreEqual(value, result2);
        }
        
        TEST_METHOD(Read64BitsDeadbeef)
        {
            bit_stream stream(20);
            bit_stream_writer writer(stream);

            uint64_t value = 0xDEADBEEFBAAAAAAD;

            writer.write_bits32(0b111, 3);
            writer.write_bits64(value, 64);
            writer.write_bits32(0b11111, 5);
            writer.commit();

            auto result32 = stream.read_bits32(3, 8);
            auto result64 = stream.read_bits64(3, 64);

            Assert::AreEqual(72, (int)stream.get_commited_bits());
            Assert::AreEqual(value, result64);
        }

        TEST_METHOD(TestLeadingZeros32)
        {
            int result = bit_utils::clz32(1);
            Assert::AreEqual(31, result);

            result = bit_utils::clz32(0xFF);
            Assert::AreEqual(24, result);

            result = bit_utils::clz32(0xFFFF);
            Assert::AreEqual(16, result);

            result = bit_utils::clz32(0b10101010101010);
            Assert::AreEqual(18, result);

            result = bit_utils::clz32(0);
            Assert::AreEqual(32, result);

            result = bit_utils::clz32(0xFFFFFFFF);
            Assert::AreEqual(0, result);
        }

        TEST_METHOD(TestLeadingZeros64)
        {
            int result = bit_utils::clz64(1);
            Assert::AreEqual(63, result);

            result = bit_utils::clz64(0xFF);
            Assert::AreEqual(56, result);

            result = bit_utils::clz64(0xFFFF);
            Assert::AreEqual(48, result);

            result = bit_utils::clz64(0b10101010101010);
            Assert::AreEqual(50, result);

            result = bit_utils::clz64(0);
            Assert::AreEqual(64, result);

            result = bit_utils::clz64(0xFFFFFFFFFFFFFFFF);
            Assert::AreEqual(0, result);
        }

        TEST_METHOD(TestTrailingZeros32)
        {
            int result = bit_utils::ctz32(1);
            Assert::AreEqual(0, result);

            result = bit_utils::ctz32(0b10);
            Assert::AreEqual(1, result);

            result = bit_utils::ctz32(0b1000);
            Assert::AreEqual(3, result);

            result = bit_utils::ctz32(0b10101010101000);
            Assert::AreEqual(3, result);

            result = bit_utils::ctz32(0);
            Assert::AreEqual(32, result);

            result = bit_utils::ctz32(0xFFFFFFFF);
            Assert::AreEqual(0, result);
            
            result = bit_utils::ctz32(1 << 31);
            Assert::AreEqual(31, result);
        }

        TEST_METHOD(TestTrailingZeros64)
        {
            int result = bit_utils::ctz64(1);
            Assert::AreEqual(0, result);

            result = bit_utils::ctz64(0b10);
            Assert::AreEqual(1, result);

            result = bit_utils::ctz64(0b1000);
            Assert::AreEqual(3, result);

            result = bit_utils::ctz64(0b10101010101000);
            Assert::AreEqual(3, result);

            result = bit_utils::ctz64(0);
            Assert::AreEqual(64, result);

            result = bit_utils::ctz64(0xF000000100000000);
            Assert::AreEqual(32, result);
            
            result = bit_utils::ctz64(0xF000000FFFFFFFFF);
            Assert::AreEqual(0, result);

            result = bit_utils::ctz64(1 << 31);
            Assert::AreEqual(31, result);
            
            result = bit_utils::ctz64((uint64_t)1 << 63);
            Assert::AreEqual(63, result);
        }
    };
}