#include "local_storage.h"
#include "src/cdb_common/not_implemented_exception.h"

namespace cdb {
namespace sg {

std::shared_ptr<iblob> local_storage::get_blob(std::string blob_name)
{
    throw cdb::not_implemented("local_storage::get_blob");
}

std::shared_ptr<iblob> local_storage::create_blob(std::string blob_name)
{
    throw cdb::not_implemented("local_storage::create_blob");
}

bool local_storage::blob_exist(std::string blob_name)
{
    throw cdb::not_implemented("local_storage::blob_exist");
}

bool local_storage::delete_blob(std::string blob_name)
{
    throw cdb::not_implemented("local_storage::delete_blob");
}

}}