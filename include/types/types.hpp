#ifndef __TYPES_H
#define __TYPES_H

#include "common/common.hpp"

enum TypeKind {
    Int,
    Bool,
    Char,
    Float,
    Void,
    Array,
    Pointer,
    Struct,
    Function
};

struct Type {
    public:
        Type(TypeKind kind)
        :kind(kind){}
        Type(TypeKind kind, size_t size)
        : kind(kind), size(size) {}

        virtual std::string to_string() = 0;
        virtual bool equals(Type* other) = 0;
        void setConst(bool is_const) {
            this->is_const == is_const;
        }

    public:
        TypeKind kind;    
        size_t size; //memory size
        bool is_const;
};
using TypePtr = std::unique_ptr<struct Type>;

struct PrimitiveType : Type {
    public:
        PrimitiveType(TypeKind kind, size_t size)
        : Type(kind, size) {}

        bool equals(Type* other) override {
            return kind == other->kind;
        }

        std::string to_string() override {
            switch(kind) {
                case TypeKind::Void: return "void";
                case TypeKind::Int: return "int";
                case TypeKind::Float: return "float";
                case TypeKind::Char: return "char";
                case TypeKind::Bool: return "bool";
                default: return "unknown";
            }
        }
    public:

};

struct ArrayType : public Type {
    ArrayType()
    :Type(TypeKind::Array) {}

    void setBaseTypeAndReverse(TypePtr ptr) {
        element_type = std::move(ptr);
        std::reverse(dims.begin(), dims.end());
    }

    void addDim(size_t n) {
        dims.push_back(n);
    }

    std::string to_string() override {
        std::string res = element_type ? element_type->to_string() : "";
        for(size_t i = 0; i < dims.size(); i++) {
            res += "[" + (dims[i] ? std::to_string(dims[i]) : "") + "]";
        }
        return res;
    }

    bool equals(Type* other) override {
        if(other == nullptr) return false;
        if(other->kind != TypeKind::Array) return false;
        ArrayType* ptr = static_cast<ArrayType*>(other);
        if(!element_type->equals(ptr->element_type.get()))
            return false;
        if(dims.size() != ptr->dims.size()) {
            return false;
        }
        for(size_t i = 0; i < dims.size(); i++) {
            if(dims[i] != ptr->dims[i]) return false;
        }
        return true;
    }

    TypeKind kind;
    std::vector<size_t> dims;
    TypePtr element_type;
};

struct TypeFactory {
    public:
        static TypePtr getTypeFromName(const std::string& type_name) {
            static const std::unordered_map<std::string, TypeKind> type_map = {
                {"void", TypeKind::Void},
                {"int", TypeKind::Int},
                {"float", TypeKind::Float},
                {"char", TypeKind::Char},
                {"bool", TypeKind::Bool}
            };

            auto it = type_map.find(type_name);
            if (it == type_map.end()) {
                throw std::runtime_error("Unknown type name: " + type_name);
            }

            TypePtr type;
            switch (it->second) {
                case TypeKind::Void: 
                    type = TypeFactory::getVoid();
                    break;
                case TypeKind::Int:
                    type = TypeFactory::getInt();
                    break;
                case TypeKind::Float:
                    type = TypeFactory::getFloat();
                    break;
                case TypeKind::Char:
                    type = TypeFactory::getChar();
                    break;
                case TypeKind::Bool:
                    type = TypeFactory::getBool();
                    break;
                default:
                    throw std::runtime_error("Unhandled type in mapper: " + type_name);
            }
            return type;
        }
        static TypePtr getVoid() { return makePrimitive(TypeKind::Void, 0); }
        static TypePtr getInt() { return makePrimitive(TypeKind::Int, 4); }
        static TypePtr getFloat() { return makePrimitive(TypeKind::Float, 4); }
        static TypePtr getChar() { return makePrimitive(TypeKind::Char, 1); }
        static TypePtr getBool() { return makePrimitive(TypeKind::Bool, 1); }

        // TypePtr getPointer(TypePtr pointee) {
        //     return std::make_unique<PointerType>(std::move(pointee));
        // }

        static TypePtr getArray() {
            return std::unique_ptr<ArrayType>(new ArrayType());
        }
    private:
        static TypePtr makePrimitive(TypeKind kind, size_t size) {
            return TypePtr(new PrimitiveType(kind, size));
        }
};

#endif