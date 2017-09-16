// Benchmark.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <memory>
#include <iostream>

#include "src\3rdparty\hayai\src\hayai.hpp"

class DeliveryMan
{
public:
    /// Deliver a package.

    /// @param distance Distance the package has to travel.
    void DeliverPackage(std::size_t distance)
    {
        // Waste some clock cycles here.
        std::size_t largeNumber = 10000u * distance / _speed;
        volatile std::size_t targetNumber;
        while (largeNumber--)
            targetNumber = largeNumber;
    }


    /// Initialize a delivery man instance.

    /// @param speed Delivery man speed from 1 to 10.
    DeliveryMan(std::size_t speed)
        : _speed(speed)
    {

    }
private:
    std::size_t _speed; ///< Delivery man speed from 1 to 10.
};


BENCHMARK(DeliveryMan, DeliverPackage, 10, 100)
{
    DeliveryMan(1).DeliverPackage(100);
}

std::wstring get_current_directory()
{
    TCHAR pwd[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, pwd);
    return std::wstring(pwd);
}

using vector_of_strings = std::shared_ptr<std::vector<std::string>>;

vector_of_strings get_all_lines(std::string file_name)
{
    std::ifstream infile(file_name);
    std::string line;
    vector_of_strings lines = std::make_shared<std::vector<std::string>>();
    while (std::getline(infile, line))
    {
        lines->push_back(line);
    }
    return lines;
}

int main()  
{
    //hayai::ConsoleOutputter consoleOutputter;
    //hayai::Benchmarker::AddOutputter(consoleOutputter);
    //hayai::Benchmarker::RunAllTests();

    auto lines = get_all_lines("data\\realAWSCloudwatch\\elb_request_count_8c0756.csv");
    for (auto& line : *lines.get())
    {
        std::cout << line.c_str() << std::endl;
    }

    return 0;
}

