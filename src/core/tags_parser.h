#pragma once

#include "src/cdb_common/common.h"

namespace cdb{
namespace core{

using tags_or_error = Result<tags_map, uint32_t>;

class tags_parser
{
public:
    static bool parse(std::string& tags, tags_map& tm);
};

}}