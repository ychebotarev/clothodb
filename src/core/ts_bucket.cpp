#include "src/core/ts_bucket.h"
#include "src/core/Compressor/Decompressor.h"

namespace clothodb {
namespace core {

ts_bucket::ts_bucket(const ts_properties& properties):
    m_properties(properties),
    m_sealed(false),
    m_stream_writer(m_stream)
{
    m_timestamp_compressor = std::make_unique<timestamp_compressor>(m_stream_writer);
    switch (m_properties.m_type)
    {
    case ts_type::TypeDouble:
    {
        m_value_compressor = std::make_unique<double_compressor>(m_stream_writer);
        break;
    }
    case ts_type::TypeInteger:
    {
        m_value_compressor = std::make_unique<integer_compressor>(m_stream_writer);
    }
    default:
        break;
    }
}

ts_bucket::~ts_bucket()
{
}

void ts_bucket::reset()
{
    m_stream.set_commited_bits(0);
    m_stream_writer.set_position(0);
    m_sealed = false;
}

void ts_bucket::add_value(uint64_t value, uint32_t timestamp)
{
    uint32_t milliseconds = 0;
    if (m_properties.m_store_milliseconds)
    {
        milliseconds = timestamp % 1000;
    }
    timestamp /= 1000;

    bool first_value = m_stream.is_empty();

    if (first_value)
    {
        m_timestamp_compressor->append_first_value(timestamp);
        m_value_compressor->append_first_value(&value);
    }
    else
    {
        m_timestamp_compressor->append_next_value(timestamp);
        m_value_compressor->append_next_value(&value);
    }

    if (m_properties.m_store_milliseconds)
    {
        m_stream_writer.write_bits32(milliseconds, 10);
    }

    m_stream_writer.commit();
}

void ts_bucket::decompress(
    std::vector<ts_point>& points, 
    uint64_t base_time_ms,
    uint64_t start_time_ms, 
    uint64_t end_time_ms)
{
    if (m_stream.is_empty())
    {
        return;
    }

    std::unique_ptr<value_decompressor> decompressor;
    bit_stream_reader reader(m_stream);

    if(m_properties.m_type == ts_type::TypeDouble)
    {
        decompressor = std::unique_ptr<double_decompressor>(new double_decompressor(reader));
    }
    if (m_properties.m_type == ts_type::TypeInteger)
    {
        decompressor = std::unique_ptr<integer_decompressor>(new integer_decompressor(reader));
    }
    
    timestamp_decompressor timestamp_dcompressor(reader);
    uint64_t timestamp = timestamp_dcompressor.get_first_value();
    timestamp *= 1000;
    timestamp += base_time_ms;

    uint64_t value = decompressor->get_first_value();
    if (m_properties.m_store_milliseconds)
    {
        uint32_t milliseconds = reader.read_bits32(10);
        timestamp += milliseconds;
    }

    if (timestamp >= start_time_ms && timestamp <= end_time_ms)
        points.push_back({ value, timestamp });

    uint32_t toDecompress = m_stream.get_commited_bits();
    while (reader.can_read() && reader.get_position() < toDecompress)
    {
        timestamp = timestamp_dcompressor.get_next_value();
        timestamp *= 1000;
        timestamp += base_time_ms;

        value = decompressor->get_next_value();

        if (m_properties.m_store_milliseconds)
        {
            uint32_t milliseconds = reader.read_bits32(10);
            timestamp += milliseconds;
        }
        if (timestamp >= start_time_ms && timestamp <= end_time_ms)
            points.push_back({ value, timestamp });
    }
}

void ts_bucket::seal()
{
    m_sealed = true;
}

}}
