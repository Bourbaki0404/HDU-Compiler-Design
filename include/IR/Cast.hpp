#pragma once
#include "common/common.hpp"

namespace IR{

struct Value;

template <typename T>
inline bool isa(const Value *V);

template <typename T>
inline T *dyn_cast(Value *V);

}