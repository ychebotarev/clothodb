#pragma once

#include "src/cdb_common/common.h"
#include "src/cdb_common/serialize_block.h"

#include "src/cdb_compressor/bit_stream.h"
#include "src/cdb_compressor/compressor.h"

#include "src/cdb_timeseries/data_point.h"
#include "src/cdb_timeseries/ts_properties.h"

namespace cdb{
namespace ts{

class ts_bucket
{
public:
    ts_bucket(const ts_properties& config);
    ~ts_bucket();

    void reset();
    void add_value(uint64_t value, uint32_t timestamp_scaled, uint32_t milliseconds);

    void decompress(
        std::vector<data_point>& points,
        uint64_t base_time_ms,
        uint64_t start_time,
        uint64_t end_time);

    void seal();
    bool is_sealed() const { return m_sealed; };

    cdb::compressor::bit_stream_reader get_stream_reader() const { return cdb::compressor::bit_stream_reader(m_stream); }
    cdb::compressor::bit_stream_writer get_stream_writer() { return cdb::compressor::bit_stream_writer(m_stream); }
    cdb::compressor::bit_stream& get_stream() { return m_stream; }
    std::unique_ptr<cdb::compressor::timestamp_compressor>& get_timestamp_compressor() { return m_timestamp_compressor; }
    std::unique_ptr<cdb::compressor::value_compressor>& get_value_compressor() { return m_value_compressor; }

    uint32_t get_commited_bits() const { return m_stream.get_commited_bits(); }

    void serialize(cdb::serialize_block& block);

private:
    cdb::compressor::bit_stream m_stream;
    cdb::compressor::bit_stream_writer m_stream_writer;

    const ts_properties& m_properties;

    std::unique_ptr<cdb::compressor::timestamp_compressor> m_timestamp_compressor;
    std::unique_ptr<cdb::compressor::value_compressor> m_value_compressor;
    bool m_sealed;
};

}}