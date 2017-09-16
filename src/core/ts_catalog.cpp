#include "src/core/ts_catalog.h"

namespace clothodb {
namespace core {

ts_catalog::ts_catalog()
{

}

ts_catalog::~ts_catalog()
{

}

std::string ts_catalog::create_time_series(
    std::shared_ptr<ts_properties> config)
{
    return "";
}

std::string ts_catalog::add_value(
    std::string guid, 
    uint64_t value, 
    uint64_t timestamp)
{
    return "";
}

}}