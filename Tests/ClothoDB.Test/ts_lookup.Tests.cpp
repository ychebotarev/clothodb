#include "stdafx.h"
#include <thread>
#include <functional>

#include "CppUnitTest.h"

#include "src/cdb_common/common.h"

#include "src/core/ts_lookup.h"

using namespace cdb::core;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ClothDBTest
{
    TEST_CLASS(ts_lookup_tests)
    {
    public:
        TEST_METHOD(ts_lookup_tests_simple_add)
        {
            ts_lookup ts;
            std::string metric_name = "metric_name";
            cdb::tags_map tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            auto add_result_positive = ts.add(metric_name, tagsMap);
            auto add_result_negative = ts.add(metric_name, tagsMap);

            Assert::IsTrue(add_result_positive.is_value());
            Assert::AreEqual(add_result_positive.as_value(), 1U);
            Assert::IsTrue(add_result_negative.is_error());

            Assert::AreEqual(ts.get_state(), 1U);
        }

        TEST_METHOD(ts_lookup_tests_multithread_add)
        {
            ts_lookup ts;
            std::string metric_name = "metric_name";
            cdb::tags_map tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            std::vector<std::thread> workers;
            for (int i = 0; i < 5; i++) 
            {
                workers.push_back(std::thread([&ts, &metric_name, &tagsMap]()
                {
                    for (int i = 0; i < 1000; ++i)
                    {
                        ts.add(metric_name, tagsMap);
                    }
                }));
            }
            for (auto& t : workers) t.join();
            
            Assert::AreEqual(ts.get_state(), 1U);
        }
        
        TEST_METHOD(ts_lookup_tests_simple_find)
        {
            ts_lookup ts;
            std::string metric_name = "metric_name";
            cdb::tags_map tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            auto created_uid = ts.add(metric_name, tagsMap);
            auto find_uid = ts.find(metric_name, tagsMap);

            tagsMap["a3"] = "a3";
            auto find_error = ts.find(metric_name, tagsMap);

            Assert::IsTrue(created_uid.is_value());
            Assert::AreEqual(created_uid.as_value(), 1U);

            Assert::IsTrue(find_uid.is_value());
            Assert::AreEqual(find_uid.as_value(), 1U);

            Assert::IsTrue(find_error.is_error());

            Assert::AreEqual(ts.get_state(), 1U);
        }
        
        TEST_METHOD(ts_lookup_tests_multithread_find)
        {
            ts_lookup ts;
            std::string metric_name_exist = "metric_name_exist";
            std::string metric_name_not_exist = "metric_name_not_exist";
            cdb::tags_map tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            auto created_uid = ts.add(metric_name_exist, tagsMap);
            int found_not_existing = 0;
            int not_found_existing = 0;

            std::vector<std::thread> workers;
            for (int i = 0; i < 5; i++)
            {
                workers.push_back(std::thread([&]()
                {
                    for (int i = 0; i < 1000; ++i)
                    {
                        auto existing = ts.find(metric_name_exist, tagsMap);
                        auto not_existing = ts.find(metric_name_not_exist, tagsMap);

                        if (existing.is_error()) ++not_found_existing;
                        if (not_existing.is_value()) ++found_not_existing;
                    }
                }));
            }
            for (auto& t : workers) t.join();

            Assert::AreEqual(0, not_found_existing);
            Assert::AreEqual(0, found_not_existing);
        }

        TEST_METHOD(ts_lookup_tests_simple_remove)
        {
            ts_lookup ts;
            std::string metric_name_1 = "metric_name_1";
            std::string metric_name_2 = "metric_name_2";
            cdb::tags_map tagsMap{ {"a1", "a1"}, { "a2", "a2" } };

            ts.add(metric_name_1, tagsMap);
            ts.add(metric_name_2, tagsMap);

            Assert::AreEqual(ts.get_state(), 2U);

            ts.remove(metric_name_2, tagsMap);
            auto result_negative = ts.find(metric_name_2, tagsMap);
            auto result_positive = ts.find(metric_name_1, tagsMap);
            
            Assert::IsTrue(result_negative.is_error());
            Assert::IsTrue(result_positive.is_value());
        }

        TEST_METHOD(ts_lookup_tests_multithread_remove)
        {
            ts_lookup ts;
            std::string metric_name_1 = "metric_name_1";
            std::string metric_name_2 = "metric_name_2";
            cdb::tags_map tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            ts.add(metric_name_1, tagsMap);
            ts.add(metric_name_2, tagsMap);

            Assert::AreEqual(2U, ts.get_state());

            std::vector<std::thread> workers;
            for (int i = 0; i < 5; i++)
            {
                workers.push_back(std::thread([&]()
                {
                    for (int i = 0; i < 1000; ++i)
                        ts.remove(metric_name_2, tagsMap);
                }));
            }
            
            for (auto& t : workers) t.join();

            auto result_negative = ts.find(metric_name_2, tagsMap);
            auto result_positive = ts.find(metric_name_1, tagsMap);

            Assert::IsTrue(result_negative.is_error());
            Assert::IsTrue(result_positive.is_value());
        }
    };
}