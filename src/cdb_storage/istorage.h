#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cdb {
namespace sg {

class iblob;
    
class istorage
{
public:
    istorage()
    {};
    
    virtual ~istorage()
    {};

    virtual std::shared_ptr<iblob> get_blob(std::string blob_name) = 0;
    virtual std::shared_ptr<iblob> create_blob(std::string blob_name) = 0;
    virtual bool blob_exist(std::string blob_name) = 0;
    virtual bool delete_blob(std::string blob_name) = 0;
};

}}