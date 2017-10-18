#include "stdafx.h"

#include <functional>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/cdb_common/serialize_block.h"
using namespace cdb;
namespace ClothDBTest
{

    TEST_CLASS(serialize_block_tests)
    {
    public:
        TEST_METHOD(serialize_block_acceptance)
        {
            std::string expected = "hello";
            auto expected_bytes = expected.length() + 1 + sizeof(size_t);
            
            serialize_block block(expected_bytes * 2);
            auto result = block.write(expected);

            Assert::IsTrue(result);
            Assert::AreEqual(expected_bytes, block.get_length());
        }
        
        TEST_METHOD(serialize_block_full)
        {
            std::string expected = "hello";
            auto expected_bytes = expected.length() + 1 + sizeof(size_t);

            serialize_block block(expected_bytes);
            auto result = block.write(expected);

            Assert::IsTrue(result);
            Assert::AreEqual(expected_bytes, block.get_length());
        }

        TEST_METHOD(serialize_block_string_overflaw)
        {
            serialize_block block(5);
            std::string str = "hello";
            auto result = block.write(str);

            Assert::IsFalse(result);
            Assert::AreEqual((size_t)0, block.get_length());
        }
        
        TEST_METHOD(serialize_block_multiple)
        {
            uint32_t value = 1;
            serialize_block block(20);
            block.write(value);
            block.write(value);
            block.write(value);
            
            Assert::AreEqual(sizeof(uint32_t) * 3, block.get_length());
        }
        
        TEST_METHOD(serialize_block_multiple_overflaw)
        {
            uint64_t value = 1;
            serialize_block block(sizeof(uint64_t) * 2 + 1);
            block.write(value);
            block.write(value);
            block.write(value);

            Assert::AreEqual(sizeof(uint64_t) * 2, block.get_length());
        }
    };
}
