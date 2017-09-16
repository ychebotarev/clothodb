#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <memory>

#include "src/common/Result.h"

using namespace clothodb::common;

namespace clothodb {
namespace core {

template<class T>
using vector_or_error = Result<std::shared_ptr<std::vector<T>>, uint32_t>;

}}