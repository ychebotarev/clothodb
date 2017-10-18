#include <vector>
#include <memory>

#include "src/cdb_compressor/bit_stream.h"
#include "src/cdb_timeseries/data_point.h"

class CompressBenchmark
{
public:
    CompressBenchmark(std::vector<cdb::ts::data_point>& expected_points)
        : m_expected_points(expected_points)
    {}

    uint64_t RunTimestampCompressTest(cdb::compressor::bit_stream& stream);
    uint64_t RunIntegerCompressTest(cdb::compressor::bit_stream& stream);
    uint64_t RunDoubleCompressTest(cdb::compressor::bit_stream& stream);

    uint64_t RunTimestampDecompressTest(cdb::compressor::bit_stream& stream);
    uint64_t RunIntegerDecompressTest(cdb::compressor::bit_stream& stream);
    uint64_t RunDoubleDecompressTest(cdb::compressor::bit_stream& stream);
private:
    std::vector<cdb::ts::data_point>& m_expected_points;
};
