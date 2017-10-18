#pragma once

#include "iblob.h"

namespace cdb {
namespace sg {

class local_blob : public iblob
{
public:
    local_blob();
    ~local_blob();

    virtual char* get_data();
};

}}

