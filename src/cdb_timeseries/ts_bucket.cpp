#include "src/cdb_common/constants.h"
#include "src/cdb_common/time_helpers.h"
#include "src/cdb_compressor/decompressor.h"

#include "src/cdb_timeseries/ts_bucket.h"

namespace cdb{
namespace ts{

using namespace std;
using namespace cdb;
using namespace compressor;

ts_bucket::ts_bucket(const ts_properties& properties):
    m_properties(properties),
    m_stream(10000),
    m_sealed(false),
    m_stream_writer(m_stream)
{
    m_timestamp_compressor = make_unique<timestamp_compressor>(m_stream_writer);
    switch (m_properties.m_type)
    {
    case ts_type::TypeDouble:
    {
        m_value_compressor = make_unique<double_compressor>(m_stream_writer);
        break;
    }
    case ts_type::TypeInteger:
    {
        m_value_compressor = make_unique<integer_compressor>(m_stream_writer);
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

void ts_bucket::add_value(uint64_t value, uint32_t timestamp_scaled, uint32_t milliseconds)
{
    bool first_value = m_stream.is_empty();

    if (first_value)
    {
        m_timestamp_compressor->append_first_value(timestamp_scaled);
        m_value_compressor->append_first_value(value);
    }
    else
    {
        m_timestamp_compressor->append_next_value(timestamp_scaled);
        m_value_compressor->append_next_value(value);
    }

    if (m_properties.m_store_milliseconds)
    {
        m_stream_writer.write_bits32(milliseconds, Constants::kMillisecondsBits);
    }

    m_stream_writer.commit();
}

//decompress is thread safe
//the only parameter that can be changed by add_value is the number of commited bits
//
void ts_bucket::decompress(
    vector<data_point>& points,
    uint64_t bucket_start_ms,
    uint64_t start_time_ms, 
    uint64_t end_time_ms)
{
    if (m_stream.is_empty())
    {
        return;
    }

    unique_ptr<value_decompressor> value_decompressor;
    bit_stream_reader reader(m_stream);

    if(m_properties.m_type == ts_type::TypeDouble)
    {
        value_decompressor = unique_ptr<double_decompressor>(new double_decompressor(reader));
    }
    else if (m_properties.m_type == ts_type::TypeInteger)
    {
        value_decompressor = unique_ptr<integer_decompressor>(new integer_decompressor(reader));
    }
    
    timestamp_decompressor timestamp_dcompressor(reader);
    uint64_t timestamp = timestamp_dcompressor.get_first_value();
    timestamp *= (int)(m_properties.m_resolution);
    timestamp += bucket_start_ms;

    uint64_t value = value_decompressor->get_first_value();
    if (m_properties.m_store_milliseconds)
    {
        uint32_t milliseconds = reader.read_bits32(Constants::kMillisecondsBits);
        timestamp += milliseconds;
    }

    if (timestamp >= start_time_ms && timestamp <= end_time_ms)
        points.push_back({ value, timestamp });

    uint32_t toDecompress = m_stream.get_commited_bits();
    while (reader.get_position() < toDecompress)
    {
        timestamp = timestamp_dcompressor.get_next_value();
        timestamp *= (int)(m_properties.m_resolution);
        timestamp += bucket_start_ms;

        value = value_decompressor->get_next_value();

        if (m_properties.m_store_milliseconds)
        {
            uint32_t milliseconds = reader.read_bits32(Constants::kMillisecondsBits);
            timestamp += milliseconds;
        }

        if (timestamp >= start_time_ms && timestamp <= end_time_ms)
        {
            points.push_back({ value, timestamp });
        }
    }
}

void ts_bucket::seal()
{
    m_sealed = true;
}

void ts_bucket::serialize(serialize_block& block)
{

}


}}
