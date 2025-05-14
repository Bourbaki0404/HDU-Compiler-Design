#include "IR/Type.hpp"

namespace IR{

IntegerType *Type::getInt1Ty() {
    return IntegerType::get(1);
}

IntegerType *Type::getInt8Ty() {
    return IntegerType::get(8);
}

IntegerType *Type::getInt16Ty() {
    return IntegerType::get(16);
}

IntegerType *Type::getInt32Ty() {
    return IntegerType::get(32);
}

IntegerType *Type::getInt64Ty() {
    return IntegerType::get(64);
}

Type *Type::getIntegerTy(size_t num_bits) {
    return IntegerType::get(num_bits);
}

Type *Type::getPointerTy(Type *Pointee) {
    return PointerType::get(Pointee);
}

Type *Type::getFunctionTy(Type *Result, const std::vector<Type*> &Params, bool IsVarArgs) {
    return FunctionType::get(Result, Params, IsVarArgs);
}

Type *Type::getArrayTy(Type *ContainedTy, size_t NumElements) {
    return ArrayType::get(ContainedTy, NumElements);
}

Type *Type::getStructTy(const std::string &n, const std::vector<Type*> &m) {
    return StructType::get(n, m);
}

Type *Type::getVoidTy() {
    static Type *VoidTy = new Type(typeKind::VoidTy);
    return VoidTy;
}

Type *Type::getLabelTy() {
    static Type *LabelTy = new Type(typeKind::LabelTy);
    return LabelTy;
}

Type *Type::getFloatTy() {
    static Type *FloatTy = new Type(typeKind::FloatTy);
    return FloatTy;
}

PointerType *Type::getPointerTo() const {
    __assert__(this, "getPointerTo fail, nullpointer");
    return PointerType::get(const_cast<Type*>(this));
}

bool Type::equals(struct Type *ty) {
    if(ty->getTypeKind() == typeKind::IntegerTy) {
        return static_cast<IntegerType*>(ty)->equals(this);
    } else if(ty->getTypeKind() == typeKind::PointerTy) {
        return static_cast<PointerType*>(ty)->equals(this);
    } else if(ty->getTypeKind() == typeKind::FunctionTy) {
        return static_cast<FunctionType*>(ty)->equals(this);
    } else if(ty->getTypeKind() == typeKind::ArrayTy) {
        return static_cast<ArrayType*>(ty)->equals(this);
    } else if(ty->getTypeKind() == typeKind::StructTy) {
        return static_cast<StructType*>(ty)->equals(this);
    } else {
        return false;
    }
}

}