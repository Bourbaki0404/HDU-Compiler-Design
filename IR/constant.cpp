#include "IR/constant.hpp"

namespace IR {

Constant *Constant::getIntegerValue(Type *Ty, const uint64_t val) {
    if(!Ty || Ty->kind != Type::typeKind::Integer) {
        std::cout << "Constant::getIntegerValue Fail\n";
        exit(1);
    }
    auto intTy = dynamic_cast<IntegerType*>(Ty);
    return new ConstantInt(intTy, val);
}























}