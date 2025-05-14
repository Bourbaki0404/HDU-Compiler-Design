#pragma once
#include "common/common.hpp"
#include "IR/utils.hpp"

namespace IR {

struct IntegerType;
struct PointerType;
struct FunctionType;
struct ArrayType;
struct StructType;
struct Type;

struct Type {

    enum typeKind {
        IntegerTy,
        PointerTy,
        ArrayTy,
        FunctionTy,
        FloatTy,
        StructTy,
        VoidTy,
        LabelTy
    };

    Type(typeKind kind) {
        this->kind = kind;
    }

    bool equals(struct Type *ty);

    typeKind getTypeKind() const { return kind; }



    /// Methods for testing the type
    /// Return true if this is an IntegerType of the given width.
    inline bool isIntegerTy(unsigned Bitwidth) const { return kind == typeKind::IntegerTy; }

    /// Return true if this is an instance of VoidType
    inline bool isVoidTy() const { return kind == typeKind::VoidTy; }

    /// Return true if this is 'float', a 32-bit IEEE fp type.
    inline bool isFloatTy() const { return kind == typeKind::FloatTy; }

    /// True if this is an instance of FunctionType.
    inline bool isFunctionTy() const { return kind == typeKind::FunctionTy; }

    /// True if this is an instance of ArrayType.
    inline bool isArrayTy() const { return kind == typeKind::ArrayTy; }

    /// True if this is an instance of PointerType.
    inline bool isPointerTy() const { return kind == typeKind::PointerTy; }

    /// Methods for creating new types
    static Type *getIntegerTy(size_t num_bits);
    static IntegerType *getInt1Ty();
    static IntegerType *getInt8Ty();
    static IntegerType *getInt16Ty();
    static IntegerType *getInt32Ty();
    static IntegerType *getInt64Ty();

    static Type *getPointerTy(Type *Pointee);
    static Type *getFunctionTy(Type *Result, const std::vector<Type*> &Params, bool IsVarArgs);
    static Type *getArrayTy(Type *ContainedTy, size_t NumElements);
    static Type *getStructTy(const std::string &n, const std::vector<Type*> &m);
    static Type *getVoidTy();
    static Type *getLabelTy();
    static Type *getFloatTy();

    /// Return a pointer to the current type. This is equivalent to
    /// PointerType::get(Foo).
    PointerType *getPointerTo() const;

    void setSubclassData(unsigned val) { SubclassData = val; }

    unsigned getSubclassData() const { return SubclassData; }

    typeKind kind;
    unsigned SubclassData = -1;
};

struct IntegerType : public Type {
    /// This enum is just used to hold constants we need for IntegerType.
    enum {
        MIN_INT_BITS = 1,        ///< Minimum number of bits that can be specified
        MAX_INT_BITS = (1<<23)   ///< Maximum number of bits that can be specified
        ///< Note that bit width is stored in the Type classes SubclassData field
        ///< which has 24 bits. SelectionDAG type legalization can require a
        ///< power of 2 IntegerType, so limit to the largest representable power
        ///< of 2, 8388608.
    };
    IntegerType(size_t num_bits) 
    : Type(typeKind::IntegerTy) {
        if(num_bits <= MAX_INT_BITS && num_bits >= MIN_INT_BITS)
            setSubclassData(num_bits);
        else {
            std::cout << "IntegerType Fail\n";
            exit(1);
        }
    }

    static IntegerType *get(size_t num_bits) {
        return new IntegerType(num_bits);
    }

    /// Get the number of bits in this IntegerType
    unsigned getBitWidth() const { return getSubclassData(); }

    bool equals(struct Type *ty) {
        if (ty->getTypeKind() != typeKind::IntegerTy)
            return false;
        auto *other = static_cast<IntegerType*>(ty);
        return getBitWidth() == other->getBitWidth();
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast.
    static bool classof(const Type *T) {
        return T->getTypeKind() == typeKind::IntegerTy;
    }
};

struct PointerType : public Type {
    PointerType(Type *Pointee) : Type(typeKind::PointerTy), PointeeTy(Pointee) {}

    static PointerType *get(Type *Pointee) {
        return new PointerType(Pointee);
    }

    bool equals(struct Type *ty) {
        if (ty->getTypeKind() != typeKind::PointerTy)
            return false;
        auto *other = static_cast<PointerType*>(ty);
        __assert__(PointeeTy && other->PointeeTy, "PointerType::equals fail, nullpointer");
        return PointeeTy->equals(other->PointeeTy);
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast.
    static bool classof(const Type *T) {
        return T->getTypeKind() == typeKind::PointerTy;
    }


    Type *getElementType() const {
        __assert__(PointeeTy, "PointerType::getElementType fail, nullpointer");
        return PointeeTy;
    }

    // If the pointer is opaque, it means that the pointer is not known at compile time
    bool isOpaque() const { return !PointeeTy; }

    Type *PointeeTy;
};

struct FunctionType : public Type {

    FunctionType(Type *Result, const std::vector<Type*> &Params, bool IsVarArgs)
        : Type(typeKind::FunctionTy), ContainedTys(), IsVarArgs(IsVarArgs) {
            ContainedTys.push_back(Result);
            for(auto &param : Params)
                ContainedTys.push_back(param);  
        }

    static FunctionType *get(Type *Result, const std::vector<Type*> &Params, bool IsVarArgs) {
        return new FunctionType(Result, Params, IsVarArgs);
    }

    bool equals(struct Type *ty) {
        if (ty->getTypeKind() != typeKind::FunctionTy)
            return false;
        auto *other = static_cast<FunctionType*>(ty);

        if (ContainedTys.size() != other->ContainedTys.size())
            return false;
        for (size_t i = 0; i < ContainedTys.size(); i++) {
            if (!ContainedTys[i]->equals(other->ContainedTys[i]))
                return false;
        }
        return IsVarArgs == other->IsVarArgs;
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast.
    static bool classof(const Type *T) {
        return T->getTypeKind() == typeKind::FunctionTy;
    }

    using param_iterator = std::vector<Type*>::iterator;
    using const_param_iterator = std::vector<Type*>::const_iterator;


    param_iterator param_begin() { return ContainedTys.begin() + 1; }
    const_param_iterator param_begin() const { return ContainedTys.begin() + 1; }
    param_iterator param_end() { return ContainedTys.end(); }
    const_param_iterator param_end() const { return ContainedTys.end(); }

    // Return the return type of the function
    Type *getReturnType() const {
        __assert__(ContainedTys.size() > 0, "FunctionType::getReturnType fail, empty ContainedTys");
        return ContainedTys[0];
    }

    /// Parameter type accessors.
    Type *getParamType(unsigned i) const { 
        __assert__(i < ContainedTys.size() - 1, "FunctionType::getParamType fail, index out of bounds");
        return ContainedTys[i+1]; 
    }

    size_t getNumParams() const {
        return ContainedTys.size() - 1;
    }

    bool isVarArg() const {
        return IsVarArgs;
    }

    std::vector<Type*> ContainedTys;
    bool IsVarArgs;
};

struct ArrayType : public Type {

    ArrayType(Type *ContainedTy, size_t NumElements)
        : Type(typeKind::ArrayTy), ContainedTy(ContainedTy), NumElements(NumElements) {}

    static ArrayType *get(Type *ContainedTy, size_t NumElements) {
        return new ArrayType(ContainedTy, NumElements);
    }

    bool equals(struct Type *ty) {
        if (ty->getTypeKind() != typeKind::ArrayTy)
            return false;
        auto *other = static_cast<ArrayType*>(ty);
        if(ContainedTy->equals(other->ContainedTy))
            return NumElements == other->NumElements;
        return false;
    }


    /// Methods for support type inquiry through isa, cast, and dyn_cast.
    static bool classof(const Type *T) {
        return T->getTypeKind() == typeKind::ArrayTy;
    }

    size_t getNumElements() const {
        return NumElements;
    }

    Type *getElementType() const {
        __assert__(ContainedTy, "ArrayType::getElementType fail, nullpointer");
        return ContainedTy;
    }

    // The type of the elements in the array
    Type *ContainedTy;
    // The number of elements in the array
    size_t NumElements;
};

struct StructType : public Type {
    std::vector<Type*> members;  // List of types in the struct
    std::string name;            // Optional name for the struct

    static StructType *get(const std::string &n, const std::vector<Type*> &m) {
        return new StructType(n, m);
    }

    StructType(const std::string &n, const std::vector<Type*> &m)
        : Type(typeKind::ArrayTy), name(n), members(m) {}

    bool equals(struct Type *ty) {
        if (ty->getTypeKind() != typeKind::StructTy)
            return false;
        auto *other = static_cast<StructType*>(ty);
        return name == other->name;
    }
    /// Methods for support type inquiry through isa, cast, and dyn_cast.
    static bool classof(const Type *T) {
        return T->getTypeKind() == typeKind::StructTy;
    }
};


}