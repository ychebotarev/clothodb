// Benchmark.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>

#undef min
#undef max
#include "src/3rdparty/hayai/src/hayai.hpp"

int main()
{
    std::ofstream ofs;
    ofs.open("benchmark.txt", std::ofstream::out | std::ofstream::app);

    hayai::ConsoleOutputter consoleOutputter;
    hayai::JsonOutputter jsonOutputter(ofs);

    hayai::Benchmarker::AddOutputter(consoleOutputter);
    hayai::Benchmarker::AddOutputter(jsonOutputter);
    hayai::Benchmarker::RunAllTests();

    ofs.close();
    std::cin.get();

    return 0;
}

