#include "src/TimeSeries/TimeSeriesBucket.h"
#include "src/TimeSeries/Compressor/Decompressor.h"

namespace incolun{
namespace clothodb{

TimeSeriesBucket::TimeSeriesBucket(const TimeSeriesConfig& config):
    m_config(config),
    m_Sealed(false),
    m_streamWriter(m_stream)
{
    m_timeStampCompressor = std::make_unique<TimestampCompressor>(m_streamWriter);
    switch (m_config.m_type)
    {
    case TimeSeriesType::TypeDouble:
    {
        m_valueCompressor = std::make_unique<DoubleCompressor>(m_streamWriter);
    }
    case TimeSeriesType::TypeInteger:
    {
        m_valueCompressor = std::make_unique<IntegerCompressor>(m_streamWriter);
    }
    default:
        break;
    }
}

TimeSeriesBucket::~TimeSeriesBucket()
{
}

void TimeSeriesBucket::Reset()
{
    m_stream.SetCommitedBits(0);
    m_streamWriter.SetPosition(0);
    m_Sealed = false;
}

void TimeSeriesBucket::AddValue(uint64_t value, uint32_t timestamp)
{
    uint32_t milliseconds = 0;
    if (m_config.m_storeMilliseconds)
    {
        milliseconds = timestamp % 1000;
    }
    timestamp /= 1000;

    bool firstValue = m_stream.IsEmpty();

    if (firstValue)
    {
        m_timeStampCompressor->AppendFirstValue(timestamp);
        m_valueCompressor->AppendFirstValue(&value);
    }
    else
    {
        m_timeStampCompressor->AppendNextValue(timestamp);
        m_valueCompressor->AppendNextValue(&value);
    }

    if (m_config.m_storeMilliseconds)
    {
        m_streamWriter.WriteBits32(milliseconds, 10);
    }

    m_streamWriter.Commit();
}

void TimeSeriesBucket::Decompress(
    std::vector<TimeSeriesPoint>& points, 
    uint64_t baseTime,
    uint64_t startTime, 
    uint64_t endTime)
{
    std::unique_ptr<ValueDecompressor> decompressor;
    BitStreamReader reader(m_stream);

    if(m_config.m_type == TimeSeriesType::TypeDouble)
    {
        decompressor = std::unique_ptr<DoubleDecompressor>(new DoubleDecompressor(reader));
    }
    if (m_config.m_type == TimeSeriesType::TypeInteger)
    {
        decompressor = std::unique_ptr<IntegerDecompressor>(new IntegerDecompressor(reader));
    }
    
    TimeStampDecompressor timeStampDecompressor(reader);
    uint64_t timestamp = timeStampDecompressor.GetFirstValue();
    timestamp += baseTime;
    timestamp *= 1000;

    if (m_config.m_storeMilliseconds)
    {
        uint32_t milliseconds = reader.ReadBits32(10);
        timestamp += milliseconds;
    }
    
    uint32_t toDecompress = m_stream.GetCommitedBits();
    uint64_t value = decompressor->GetFirstValue();
    if (timestamp >= startTime && timestamp <= endTime)
        points.push_back({ timestamp, value });

    while (reader.CanRead() && reader.GetPosition() < toDecompress)
    {
        timestamp = timeStampDecompressor.GetNextValue();
        timestamp += baseTime;
        timestamp *= 1000;
        if (m_config.m_storeMilliseconds)
        {
            uint32_t milliseconds = reader.ReadBits32(10);
            timestamp += milliseconds;
        }
        value = decompressor->GetNextValue();
        if (timestamp >= startTime && timestamp <= endTime)
            points.push_back({ timestamp, value });
    }
}

void TimeSeriesBucket::Seal()
{
    m_Sealed = true;
}

}}
