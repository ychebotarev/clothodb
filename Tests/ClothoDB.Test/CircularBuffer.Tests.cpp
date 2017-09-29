#include "stdafx.h"

#include <functional>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/cdb_common/circular_buffer.h"

namespace ClothDBTest
{
    TEST_CLASS(circular_buffer_tests)
    {
    public:
        TEST_METHOD(circular_buffer_size_test)
        {
            cdb::circular_buffer<int> buffer(10, GetterImpl, ResetImpl);
            buffer.set_head_index(0);
            buffer.set_tail_index(0);
            Assert::AreEqual(1, (int)buffer.size());

            buffer.set_head_index(1);
            buffer.set_tail_index(0);
            Assert::AreEqual(2, (int)buffer.size());
            
            buffer.set_head_index(9);
            buffer.set_tail_index(0);
            Assert::AreEqual(10, (int)buffer.size());
            
            buffer.set_head_index(9);
            buffer.set_tail_index(9);
            Assert::AreEqual(1, (int)buffer.size());
            
            buffer.set_head_index(10);
            buffer.set_tail_index(10);
            Assert::AreEqual(1, (int)buffer.size());

            buffer.set_head_index(0);
            buffer.set_tail_index(1);
            Assert::AreEqual(10, (int)buffer.size());

            buffer.set_head_index(0);
            buffer.set_tail_index(5);
            Assert::AreEqual(6, (int)buffer.size());
            
            buffer.set_head_index(0);
            buffer.set_tail_index(2);
            Assert::AreEqual(9, (int)buffer.size());
        }

        TEST_METHOD(circular_buffer_push_head_test)
        {
            circular_buffer_tests::sValue = 0;
            cdb::circular_buffer<int> buffer(10, GetterImpl, ResetImpl);
            buffer.move_head_forward();
            Assert::AreEqual(1, (int)buffer.size());
            for (int i = 1; i < 10; ++i)
            {
                buffer.move_head_forward();
                Assert::AreEqual(1 + i, (int)buffer.size());
            }
            //catch tail
            for (int i = 0; i < 10; ++i)
            {
                buffer.move_head_forward();
                Assert::AreEqual(10, (int)buffer.size());
            }
        }

        TEST_METHOD(circular_buffer_push_tail_test)
        {
            circular_buffer_tests::sValue = 0;
            cdb::circular_buffer<int> buffer(10, GetterImpl, ResetImpl);
            
            check_exception([&buffer]() { buffer.move_tail_forward(); }, true);
            check_exception([&buffer]() 
            { 
                buffer.move_head_forward();
                buffer.move_tail_forward();
            }, false);
        }

        TEST_METHOD(circular_buffer_get_test)
        {
            circular_buffer_tests::sValue = 0;
            cdb::circular_buffer<int>::item_resetter<int> resetter = [](int&) {};
            cdb::circular_buffer<int> buffer(10, GetterImpl, resetter);
            for (int i = 0; i < 10; ++i)
            {
                circular_buffer_tests::sValue = i;
                buffer.move_head_forward();
                auto v = buffer.head();
                Assert::AreEqual(i, (int)v);
            }
        }

    private:
        void check_exception(std::function<void()> func, bool expectException)
        {
            bool exceptionHappen = false;
            try
            {
                func();
            }
            catch (const std::out_of_range&)
            {
                exceptionHappen = true;
            }
            Assert::AreEqual(expectException, exceptionHappen);
        }

        static int sValue;
        
        static int GetterImpl()
        {
            return sValue;
        }

        static void ResetImpl(int& value)
        {
            value = value ^ 2;
        }
    };
    
    int circular_buffer_tests::sValue = 0;
}