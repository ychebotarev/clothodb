#pragma once

#include "src/cdb_common/common.h"

namespace cdb{
namespace db{

using tags_or_error = Result<std::map<std::string, std::string>, uint32_t>;

class tags_parser
{
public:
    static bool parse(const std::string& tags, const std::map<std::string, std::string>& tm);
};

}}