#include "IR/Cast.hpp"

namespace IR{

template <typename T>
inline bool isa(Value *V) {
  return V && T::classof(V);
}

// template <typename T>
// inline bool isa(Value &V) {
//   return isa<T>(&V);
// }

template <typename T>
inline T *dyn_cast(Value *V) {
  if (isa<T>(V))
    return static_cast<T *>(V);
  return nullptr;
}

// template <typename T>
// inline T &cast(Value &V) {
//   assert(isa<T>(V) && "cast<> to incorrect type!");
//   return static_cast<T &>(V);
// }

}

