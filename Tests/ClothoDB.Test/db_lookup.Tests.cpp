#include "stdafx.h"
#include <thread>
#include <functional>

#include "CppUnitTest.h"

#include "src/cdb_common/common.h"

#include "src/cdb_database/lookup.h"

using namespace cdb::db;
using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ClothDBTest
{
    TEST_CLASS(db_lookup_tests)
    {
    public:
        TEST_METHOD(db_lookup_tests_register_timeseries_singlethread)
        {
            lookup db_lookup;
            string namespace_name = "namespace_name";
            string metric_name = "metric_name";
            map<string, string> tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            auto add_result_positive = db_lookup.register_timeseries(namespace_name, metric_name, tagsMap);
            auto add_result_negative = db_lookup.register_timeseries(namespace_name, metric_name, tagsMap);

            Assert::IsTrue(add_result_positive.is_value());
            Assert::AreEqual(add_result_positive.as_value(), 1U);
            Assert::IsTrue(add_result_negative.is_error());

            Assert::AreEqual(db_lookup.get_state(), 1U);
        }

        TEST_METHOD(db_lookup_tests_register_timeseries_multithread)
        {
            lookup db_lookup;
            string namespace_name = "namespace_name";
            string metric_name = "metric_name";
            map<string, string> tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            std::vector<std::thread> workers;
            for (int i = 0; i < 5; i++) 
            {
                workers.push_back(std::thread([&db_lookup, &metric_name, &tagsMap, &namespace_name]()
                {
                    for (int i = 0; i < 1000; ++i)
                    {
                        db_lookup.register_timeseries(namespace_name, metric_name, tagsMap);
                    }
                }));
            }
            for (auto& t : workers) t.join();
            
            Assert::AreEqual(db_lookup.get_state(), 1U);
        }
        
        TEST_METHOD(db_lookup_tests_get_uid_singlethread)
        {
            lookup db_lookup;
            string namespace_name = "namespace_name";
            string metric_name = "metric_name";
            map<string, string> tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            auto created_uid = db_lookup.register_timeseries(namespace_name, metric_name, tagsMap);
            auto find_uid = db_lookup.get_uid(namespace_name, metric_name, tagsMap);

            tagsMap["a3"] = "a3";
            auto find_error = db_lookup.get_uid(namespace_name, metric_name, tagsMap);

            Assert::IsTrue(created_uid.is_value());
            Assert::AreEqual(created_uid.as_value(), 1U);

            Assert::IsTrue(find_uid.is_value());
            Assert::AreEqual(find_uid.as_value(), 1U);

            Assert::IsTrue(find_error.is_error());
            Assert::AreEqual(db_lookup.get_state(), 1U);
        }
        
        TEST_METHOD(db_lookup_tests_get_uid_multithread)
        {
            lookup db_lookup;
            string namespace_name = "namespace_name";
            string metric_name_exist = "metric_name_exist";
            string metric_name_not_exist = "metric_name_not_exist";
            map<string, string> tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            auto created_uid = db_lookup.register_timeseries(namespace_name, metric_name_exist, tagsMap);
            int found_not_existing = 0;
            int not_found_existing = 0;

            std::vector<std::thread> workers;
            for (int i = 0; i < 5; i++)
            {
                workers.push_back(std::thread([&]()
                {
                    for (int i = 0; i < 1000; ++i)
                    {
                        auto existing = db_lookup.get_uid(namespace_name, metric_name_exist, tagsMap);
                        auto not_existing = db_lookup.get_uid(namespace_name, metric_name_not_exist, tagsMap);

                        if (existing.is_error()) ++not_found_existing;
                        if (not_existing.is_value()) ++found_not_existing;
                    }
                }));
            }
            for (auto& t : workers) t.join();

            Assert::AreEqual(0, not_found_existing);
            Assert::AreEqual(0, found_not_existing);
        }

        TEST_METHOD(db_lookup_tests_unregister_timeseries_singlethread)
        {
            lookup db_lookup;
            string namespace_name = "namespace_name";
            string metric_name_1 = "metric_name_1";
            string metric_name_2 = "metric_name_2";
            map<string, string> tagsMap{ {"a1", "a1"}, { "a2", "a2" } };

            db_lookup.register_timeseries(namespace_name, metric_name_1, tagsMap);
            db_lookup.register_timeseries(namespace_name, metric_name_2, tagsMap);

            Assert::AreEqual(db_lookup.get_state(), 2U);

            db_lookup.unregister_timeseries(namespace_name, metric_name_2, tagsMap);
            auto result_negative = db_lookup.get_uid(namespace_name, metric_name_2, tagsMap);
            auto result_positive = db_lookup.get_uid(namespace_name, metric_name_1, tagsMap);
            
            Assert::IsTrue(result_negative.is_error());
            Assert::IsTrue(result_positive.is_value());
        }

        TEST_METHOD(db_lookup_tests_unregister_timeseries_multithread)
        {
            lookup db_lookup;
            string namespace_name = "namespace_name";
            string metric_name_1 = "metric_name_1";
            string metric_name_2 = "metric_name_2";
            map<string, string> tagsMap{ { "a1", "a1" }, { "a2", "a2" } };

            db_lookup.register_timeseries(namespace_name, metric_name_1, tagsMap);
            db_lookup.register_timeseries(namespace_name, metric_name_2, tagsMap);

            Assert::AreEqual(2U, db_lookup.get_state());

            std::vector<std::thread> workers;
            for (int i = 0; i < 5; i++)
            {
                workers.push_back(std::thread([&]()
                {
                    for (int i = 0; i < 1000; ++i)
                        db_lookup.unregister_timeseries(namespace_name, metric_name_2, tagsMap);
                }));
            }
            
            for (auto& t : workers) t.join();

            auto result_negative = db_lookup.get_uid(namespace_name, metric_name_2, tagsMap);
            auto result_positive = db_lookup.get_uid(namespace_name, metric_name_1, tagsMap);

            Assert::IsTrue(result_negative.is_error());
            Assert::IsTrue(result_positive.is_value());
        }
    };
}