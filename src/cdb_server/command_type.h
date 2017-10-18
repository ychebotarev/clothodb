#pragma once

namespace cdb {
namespace svr{

enum command_type
{
    unknown = 0,
    create_namespace = 1,
    delete_namespace = 2,
    create_timeseries = 3,
    delete_timeseries = 4,
    get_uid = 5,
    add_value = 6
};

}}