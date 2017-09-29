#include "stdafx.h"

#include <functional>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "src/core/ts_properties.h"

using namespace cdb::core;

namespace ClothDBTest
{
    TEST_CLASS(ts_properties_tests)
    {
    public:
        TEST_METHOD(ts_properties_tests_compare_equal)
        {
            ts_properties t1, t2;
            t1.m_metric = "t1";
            t2.m_metric = "t1";

            bool result = t1.compare(t2);
            Assert::IsTrue(result);

            t1.m_tags["a1"] = "a1";
            t2.m_tags["a1"] = "a1";
            t1.m_tags["b1"] = "b1";
            t2.m_tags["b1"] = "b1";

            result = t1.compare(t2);
            Assert::IsTrue(result);
        }
        TEST_METHOD(ts_properties_tests_compare_not_equal)
        {
            ts_properties t1, t2;
            t1.m_metric = "t1";
            t2.m_metric = "t2";

            //different metric name, no tags
            bool result = t1.compare(t2);
            Assert::IsFalse(result);

            t1.m_tags["a1"] = "a1";
            t2.m_tags["a1"] = "a1";
            t1.m_tags["b1"] = "b1";
            t2.m_tags["b1"] = "b1";

            //different metric name, same tags
            result = t1.compare(t2);
            Assert::IsFalse(result);

            //same metric name, different tags size
            t1.m_metric = "t1";
            t2.m_metric = "t1";

            t1.m_tags.clear();
            t2.m_tags.clear();

            t1.m_tags["a1"] = "a1";
            t2.m_tags["a1"] = "a1";
            t1.m_tags["b1"] = "b1";
            result = t1.compare(t2);
            Assert::IsFalse(result);
            
            //same metric name, same tags size, different tags values
            t1.m_tags.clear();
            t2.m_tags.clear();

            t1.m_tags["a1"] = "a1";
            t2.m_tags["a1"] = "a1";
            t1.m_tags["b1"] = "b1";
            t2.m_tags["b1"] = "b2";
            result = t1.compare(t2);
            Assert::IsFalse(result);
            
            //same metric name, same tags size, different tags keys
            t1.m_tags.clear();
            t2.m_tags.clear();

            t1.m_tags["a1"] = "a1";
            t2.m_tags["a1"] = "a1";
            t1.m_tags["b1"] = "b1";
            t2.m_tags["b2"] = "b1";
            result = t1.compare(t2);
            Assert::IsFalse(result);
        }
    };
}