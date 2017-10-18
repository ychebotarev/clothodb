#pragma once

#include "istorage.h"

namespace cdb {
namespace sg {

class local_storage : public istorage
{
public:
    virtual std::shared_ptr<iblob> get_blob(std::string blob_name);
    virtual std::shared_ptr<iblob> create_blob(std::string blob_name);
    virtual bool blob_exist(std::string blob_name);
    virtual bool delete_blob(std::string blob_name);

};

}}