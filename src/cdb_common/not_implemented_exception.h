#include <stdexcept>
#include "strfmt.h"

namespace cdb {

class not_implemented : public std::logic_error
{
public:
    not_implemented(const char* function_name) : std::logic_error(
        sstrfmt("Function %s not yet implemented").fmt(function_name)) 
    { };
};

}