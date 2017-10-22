#include "stdafx.h"
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <iostream>
#include <chrono>

#include <time.h>

#include "CompressBenchmark.h"

#include "src/cdb_common/time_fmt.h"
#include "src/cdb_common/time_helpers.h"
#include "src/cdb_timeseries/time_series.h"
#include "src/cdb_compressor/compressor.h"
#include "src/cdb_compressor/decompressor.h"

using namespace cdb::ts;
using namespace cdb::compressor;

uint64_t CompressBenchmark::RunTimestampCompressTest(bit_stream& stream)
{
    bit_stream_writer writer(stream);
    timestamp_compressor compressor(writer);

    auto started = std::chrono::high_resolution_clock::now();

    compressor.append_first_value(0);
    for (auto& point : m_expected_points)
    {
        compressor.append_next_value((uint32_t)point.timestamp / 1000);
    }
    writer.commit();

    auto done = std::chrono::high_resolution_clock::now();
    std::cout << "Timestamp compression: " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << " ms. Size: " << stream.get_commited_bits() << std::endl;

    return stream.get_commited_bits();
}

uint64_t CompressBenchmark::RunIntegerCompressTest(bit_stream& stream)
{
    bit_stream_writer writer(stream);
    integer_compressor compressor(writer);

    auto started = std::chrono::high_resolution_clock::now();

    compressor.append_first_value(0);
    for (auto& point : m_expected_points)
    {
        compressor.append_next_value(point.value);
    }
    writer.commit();

    auto done = std::chrono::high_resolution_clock::now();
    std::cout << "Integer compression: " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << " ms. Size: " << stream.get_commited_bits() << std::endl;

    return stream.get_commited_bits();
}

uint64_t CompressBenchmark::RunDoubleCompressTest(bit_stream& stream)
{
    bit_stream_writer writer(stream);
    double_compressor compressor(writer);

    auto started = std::chrono::high_resolution_clock::now();

    compressor.append_first_value(0);
    for (auto& point : m_expected_points)
    {
        compressor.append_next_value(point.value);
    }
    writer.commit();

    auto done = std::chrono::high_resolution_clock::now();
    std::cout << "Double compression: " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << " ms. Size: " << stream.get_commited_bits() << std::endl;

    return stream.get_commited_bits();

}

uint64_t CompressBenchmark::RunTimestampDecompressTest(bit_stream& stream)
{
    uint64_t value = 0;
    bit_stream_reader reader(stream);
    timestamp_decompressor decompressor(reader);

    auto started = std::chrono::high_resolution_clock::now();
    value += decompressor.get_first_value();
    while (reader.can_read())
    {
        value += decompressor.get_next_value();
    }
    auto done = std::chrono::high_resolution_clock::now();
    std::cout << "Timestamp decompression: " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << " ms. Result: " << value << std::endl;
    return value;
}

uint64_t CompressBenchmark::RunIntegerDecompressTest(bit_stream& stream)
{
    uint64_t value = 0;
    bit_stream_reader reader(stream);
    integer_decompressor decompressor(reader);

    auto started = std::chrono::high_resolution_clock::now();
    value += decompressor.get_first_value();
    while (reader.can_read())
    {
        value += decompressor.get_next_value();
    }
    auto done = std::chrono::high_resolution_clock::now();
    std::cout << "Timestamp decompression: " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << " ms. Result: " << value << std::endl;
    return value;
}

uint64_t CompressBenchmark::RunDoubleDecompressTest(bit_stream& stream)
{
    uint64_t value = 0;
    bit_stream_reader reader(stream);
    double_decompressor decompressor(reader);

    auto started = std::chrono::high_resolution_clock::now();
    value += decompressor.get_first_value();
    while (reader.can_read())
    {
        value += decompressor.get_next_value();
    }
    auto done = std::chrono::high_resolution_clock::now();
    std::cout << "Double decompression: " << std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
    std::cout << " ms. Result: " << value << std::endl;
    return value;

}
