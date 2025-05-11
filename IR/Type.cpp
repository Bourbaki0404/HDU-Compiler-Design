#include "IR/Type.hpp"

namespace IR{



bool IntegerType::equals(struct Type *ty) {
    return (ty->kind == kind);
}

bool ArrayType::equals(struct Type *ty) {
    return false;
}

bool FunctionType::equals(struct Type *ty) {
    return false;
}

bool PointerType::equals(struct Type *ty) {
    return false;
}



}