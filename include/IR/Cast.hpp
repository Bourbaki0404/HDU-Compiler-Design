#pragma once
#include "common/common.hpp"

namespace IR{

struct Value;
struct Type;

template <typename T>
inline bool isa(const Value *V) {
	Value *v = (Value*)V;
    return v && T::classof(v);
}

template <typename T>
inline bool isa(const Type *Ty) {
	Type *ty = (Type*)Ty;
    return ty && T::classof(ty);
}

template <typename T>
inline T *dyn_cast(const Value *V) {
	Value *v = (Value*)(V);
	if (isa<T>(v))
		return static_cast<T *>(v);
	return nullptr;
}

template <typename T>
inline T *dyn_cast(const Type *Ty) {
	Type *ty = (Type*)Ty;
	if (isa<T>(ty))
		return static_cast<T *>(ty);
	return nullptr;
}


}