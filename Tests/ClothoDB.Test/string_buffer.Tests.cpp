#include "stdafx.h"

#include <functional>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/cdb_common/string_buffer.h"

namespace ClothDBTest
{
    TEST_CLASS(string_buffer_tests)
    {
    public:
        TEST_METHOD(string_buffer_acceptance)
        {
            std::string expected = "hello";
            cdb::string_buffer buffer(10);
            buffer.append(expected.c_str());

            std::string result(buffer.get_buffer());

            Assert::AreEqual(expected.length(), buffer.get_length());
            Assert::AreEqual(expected, result);
        }
        
        TEST_METHOD(string_buffer_full)
        {
            std::string expected = "hello";
            cdb::string_buffer buffer(6);
            buffer.append(expected.c_str());

            std::string result(buffer.get_buffer());

            Assert::AreEqual(expected.length(), buffer.get_length());
            Assert::AreEqual(expected, result);
        }

        TEST_METHOD(string_buffer_overflaw)
        {
            cdb::string_buffer buffer(5);
            buffer.append("hello");

            Assert::AreEqual((size_t)0, buffer.get_length());
        }
        
        TEST_METHOD(string_buffer_multiple)
        {
            std::string expected = "hello world";
            cdb::string_buffer buffer(20);
            buffer.append("hello");
            buffer.append(" ");
            buffer.append("world");
            
            std::string result(buffer.get_buffer());

            Assert::AreEqual(expected.length(), buffer.get_length());
            Assert::AreEqual(expected, result);
        }
        
        TEST_METHOD(string_buffer_multiple_overflaw)
        {
            std::string expected = "hello ";
            cdb::string_buffer buffer(11);
            buffer.append("hello");
            buffer.append(" ");
            buffer.append("world");

            std::string result(buffer.get_buffer());

            Assert::AreEqual(expected.length(), buffer.get_length());
            Assert::AreEqual(expected, result);
        }
    };
}
