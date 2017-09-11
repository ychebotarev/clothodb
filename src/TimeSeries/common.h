#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <memory>

#include "src/common/Result.h"

namespace incolun {
namespace clothodb {

template<class T>
using VectorOrError = Result<std::shared_ptr<std::vector<T>>, uint32_t>;

}}