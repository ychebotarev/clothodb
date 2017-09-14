#include "stdafx.h"

#include <functional>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/Common/CircularBuffer.h"

using namespace incolun::clothodb;

namespace ClothDBTest
{
    TEST_CLASS(CircularBufferTests)
    {
    public:
        TEST_METHOD(CircularBufferSizeTest)
        {
            CircularBuffer<int> buffer(10, GetterImpl, ResetImpl);
            buffer.SetHeadIndex(0);
            buffer.SetTailIndex(0);
            Assert::AreEqual(1, (int)buffer.Size());

            buffer.SetHeadIndex(1);
            buffer.SetTailIndex(0);
            Assert::AreEqual(2, (int)buffer.Size());
            
            buffer.SetHeadIndex(9);
            buffer.SetTailIndex(0);
            Assert::AreEqual(10, (int)buffer.Size());
            
            buffer.SetHeadIndex(9);
            buffer.SetTailIndex(9);
            Assert::AreEqual(1, (int)buffer.Size());
            
            buffer.SetHeadIndex(10);
            buffer.SetTailIndex(10);
            Assert::AreEqual(1, (int)buffer.Size());

            buffer.SetHeadIndex(0);
            buffer.SetTailIndex(1);
            Assert::AreEqual(10, (int)buffer.Size());

            buffer.SetHeadIndex(0);
            buffer.SetTailIndex(5);
            Assert::AreEqual(6, (int)buffer.Size());
        }

        TEST_METHOD(CircularBufferPushHeadTest)
        {
            CircularBufferTests::sValue = 0;
            CircularBuffer<int> buffer(10, GetterImpl, ResetImpl);
            buffer.MoveHeadForward();
            Assert::AreEqual(1, (int)buffer.Size());
            for (int i = 1; i < 10; ++i)
            {
                buffer.MoveHeadForward();
                Assert::AreEqual(1 + i, (int)buffer.Size());
            }
            //catch tail
            for (int i = 0; i < 10; ++i)
            {
                buffer.MoveHeadForward();
                Assert::AreEqual(10, (int)buffer.Size());
            }
        }

        TEST_METHOD(CircularBufferPushTailTest)
        {
            CircularBufferTests::sValue = 0;
            CircularBuffer<int> buffer(10, GetterImpl, ResetImpl);
            
            CheckException([&buffer]() { buffer.MoveTailForward(); }, true);
            CheckException([&buffer]() 
            { 
                buffer.MoveHeadForward();
                buffer.MoveTailForward();
            }, false);
        }

        TEST_METHOD(CircularBufferAtTest)
        {
            CircularBufferTests::sValue = 0;
            CircularBuffer<int>::Resetter<int> resetter = [](int&) {};
            CircularBuffer<int> buffer(10, GetterImpl, resetter);
            for (int i = 0; i < 10; ++i)
            {
                CircularBufferTests::sValue = i;
                buffer.MoveHeadForward();
                auto v = buffer.Head();
                Assert::AreEqual(i, (int)v);
            }
        }

    private:
        void CheckException(std::function<void()> func, bool expectException)
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
    
    int CircularBufferTests::sValue = 0;
}