#include "local_blob.h"
#include "src/cdb_common/not_implemented_exception.h"

namespace cdb {
namespace sg {

local_blob::local_blob()
{
}


local_blob::~local_blob()
{
}

char* local_blob::get_data()
{
    throw cdb::not_implemented("local_blob::get_data");
}

}}
