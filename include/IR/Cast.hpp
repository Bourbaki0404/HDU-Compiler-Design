#pragma once
#include "common/common.hpp"

namespace IR{

struct Value;

template <typename T>
inline bool isa(const Value *V) {
	Value *v = (Value*)V;
    return v && T::classof(v);
}

template <typename T>
inline T *dyn_cast(const Value *V) {
	Value *v = (Value*)(V);
	if (isa<T>(v))
		return static_cast<T *>(v);
	return nullptr;
}


}