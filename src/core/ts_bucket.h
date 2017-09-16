#pragma once

#include "src/core/common.h"

#include "src/core/bit_stream.h"
#include "src/core/ts_point.h"
#include "src/core/ts_properties.h"

#include "src/core/compressor/compressor.h"

namespace clothodb {
namespace core {

class ts_bucket
{
public:
    ts_bucket(const ts_properties& config);
    ~ts_bucket();

    void reset();
    void add_value(uint64_t value, uint32_t timestamp);

    void decompress(
        std::vector<ts_point>& points, 
        uint64_t base_time_ms,
        uint64_t start_time,
        uint64_t end_time);

    void seal();
    bool is_sealed() const { return m_sealed; };

    bit_stream_reader get_stream_reader() const { return bit_stream_reader(m_stream); }
    uint32_t get_commited_bits() const { return m_stream.get_commited_bits(); }
private:
    bit_stream m_stream;
    bit_stream_writer m_stream_writer;

    const ts_properties& m_properties;

    std::unique_ptr<timestamp_compressor> m_timestamp_compressor;
    std::unique_ptr<value_compressor> m_value_compressor;
    bool m_sealed;
};

}}