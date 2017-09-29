#pragma once

#include "src/cdb_common/common.h"

#include "src/cdb_compressor/bit_stream.h"
#include "src/cdb_compressor/compressor.h"

#include "src/core/ts_point.h"
#include "src/core/ts_properties.h"

namespace cdb{
namespace core{

class ts_bucket
{
public:
    ts_bucket(const ts_properties& config);
    ~ts_bucket();

    void reset();
    void add_value(uint64_t value, uint32_t timestamp_scaled, uint32_t milliseconds);

    void decompress(
        std::vector<ts_point>& points, 
        uint64_t base_time_ms,
        uint64_t start_time,
        uint64_t end_time);

    void seal();
    bool is_sealed() const { return m_sealed; };

    compressor::bit_stream_reader get_stream_reader() const { return compressor::bit_stream_reader(m_stream); }
    uint32_t get_commited_bits() const { return m_stream.get_commited_bits(); }
private:
    cdb::compressor::bit_stream m_stream;
    cdb::compressor::bit_stream_writer m_stream_writer;

    const ts_properties& m_properties;

    std::unique_ptr<cdb::compressor::timestamp_compressor> m_timestamp_compressor;
    std::unique_ptr<cdb::compressor::value_compressor> m_value_compressor;
    bool m_sealed;
};

}}