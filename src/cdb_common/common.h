#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "src/cdb_common/Result.h"

namespace cdb{

template<class T>
using vector_or_error = Result<std::shared_ptr<std::vector<T>>, uint32_t>;

using string_or_error = Result<std::string, uint32_t>;
using uint_or_error = Result<std::uint32_t, uint32_t>;
using bool_or_error = Result<bool, uint32_t>;

using tags_map = std::map<std::string, std::string>;

}