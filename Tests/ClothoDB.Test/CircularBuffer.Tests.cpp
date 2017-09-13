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
            CircularBuffer<std::string, GetterImpl, ResetImpl> buffer(10);
            buffer.SetHeadIndex(0);
            buffer.SetTailIndex(0);
            Assert::AreEqual(1, (int)buffer.Size());

            buffer.SetHeadIndex(0);
            buffer.SetTailIndex(1);
            Assert::AreEqual(2, (int)buffer.Size());
        }
    private:
        static std::string GetterImpl()
        {
            return "Hello";
        }

        static void ResetImpl(std::string& str)
        {
            str = "";
        }
    };
}
