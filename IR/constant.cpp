#include "IR/constant.hpp"
#include "IR/Type.hpp"
#include "IR/Cast.hpp"

namespace IR {

Constant *Constant::getIntegerValue(Type *Ty, const uint64_t val) {
    if(!Ty || Ty->getTypeKind() != Type::typeKind::IntegerTy) {
        std::cout << "Constant::getIntegerValue Fail\n";
        exit(1);
    }
    auto intTy = dyn_cast<IntegerType>(Ty);
    return new ConstantInt(intTy, val);
}























}