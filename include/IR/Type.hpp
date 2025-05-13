#pragma once
#include "common/common.hpp"

namespace IR {

struct Type {

    enum typeKind {
        Integer,
        Pointer,
        Array,
        Function,
        Float,
        Struct,
        Void,
        Label
    };

    Type(typeKind kind) {
        this->kind = kind;
    }

    virtual bool equals(struct Type *ty) {
        return ty->kind == kind;
    }

    /// Return true if this is an IntegerType of the given width.
    bool isIntegerTy(unsigned Bitwidth) const { return kind == typeKind::Integer; }

    /// Return true if this is an instance of VoidType
    bool isVoidTy() const { return kind == typeKind::Void; }

    /// Return true if this is 'float', a 32-bit IEEE fp type.
    bool isFloatTy() const { return kind == typeKind::Float; }

    /// True if this is an instance of FunctionType.
    bool isFunctionTy() const { return kind == typeKind::Function; }

    /// True if this is an instance of ArrayType.
    bool isArrayTy() const { return kind == typeKind::Array; }

    /// True if this is an instance of PointerType.
    bool isPointerTy() const { return kind == typeKind::Pointer; }

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
    : Type(typeKind::Integer) {
        if(num_bits <= MAX_INT_BITS && num_bits >= MIN_INT_BITS)
            setSubclassData(num_bits);
        else {
            std::cout << "IntegerType Fail\n";
            exit(1);
        }
    }

    /// Get the number of bits in this IntegerType
    unsigned getBitWidth() const { return getSubclassData(); }

    bool equals(struct Type *ty);

};

struct PointerType : public Type {
    bool equals(struct Type *ty);
};

struct FunctionType : public Type {
    bool equals(struct Type *ty);
};

struct ArrayType : public Type {
    bool equals(struct Type *ty);
};

struct StructType : public Type {
    std::vector<Type*> members;  // List of types in the struct
    std::string name;            // Optional name for the struct

    StructType(const std::string &n, const std::vector<Type*> &m)
        : Type(typeKind::Array), name(n), members(m) {}

    bool equals(struct Type *ty) override {
        if (ty->kind != typeKind::Struct)
            return false;
        auto *other = static_cast<StructType*>(ty);
        return name == other->name;
    }
};

struct TypeFactory {
    static Type *getInt32Ty() {
        Type *ty = new IntegerType(32);
        return ty;
    }
    static Type *getInt1Ty() {
        Type *ty = new IntegerType(1);
        return ty;
    }
    static Type *getVoidTy() {
        return new Type(Type::typeKind::Void);
    }
    static Type *getLabelTy() {
        return new Type(Type::typeKind::Label);
    }
};


}