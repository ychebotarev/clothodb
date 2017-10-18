#pragma once

namespace cdb {
namespace sg {

class iblob
{
public:

    iblob()
    {}

    virtual ~iblob()
    {}

    virtual char* get_data() = 0;
};

}}