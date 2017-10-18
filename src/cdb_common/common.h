#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <SDKDDKVer.h>

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

template<class T>
using pointer_or_error = Result<std::shared_ptr<T>, uint32_t>;

using string_or_error = Result<std::string, uint32_t>;
using uint_or_error = Result<std::uint32_t, uint32_t>;
using bool_or_error = Result<bool, uint32_t>;

}