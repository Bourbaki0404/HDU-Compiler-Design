#ifndef __TYPES_H
#define __TYPES_H

#include "common/common.hpp"
#include "types/TypeChecker.hpp"

//forward decl
struct node;
struct program;
struct expr;
struct stmt;
struct lval_expr;
struct block_stmt;
struct func_param;
struct func_def;
struct init_val;
using progPtr = std::unique_ptr<program>;
using nodePtr = std::unique_ptr<node>;
using expPtr = std::unique_ptr<expr>;
using lvalPtr = std::unique_ptr<lval_expr>;
using stmtPtr = std::unique_ptr<stmt>;
using blockPtr = std::unique_ptr<block_stmt>;
using funcparamPtr = std::unique_ptr<func_param>;
using funcDefPtr = std::unique_ptr<func_def>;  
using initValPtr = std::unique_ptr<init_val>;

enum TypeKind {
    Int,
    Bool,
    Char,
    Float,
    Void,
    Array,
    Pointer,
    Class,
    Function,
    Error // when type error happens
};

struct Type {
    public:
        Type(TypeKind kind);
        Type(TypeKind kind, size_t size);
        virtual std::string to_string() = 0;
        virtual bool equals(Type* other) = 0;
        virtual void setConst() = 0;
        virtual void evaluate(TypeChecker *ptr) = 0;

    public:
        TypeKind kind;    
        size_t size; //memory size
        bool is_const = false;
};
using TypePtr = std::unique_ptr<struct Type>;

struct PrimitiveType : Type {
    public:
        PrimitiveType(TypeKind kind, size_t size);
        bool equals(Type* other) override;
        std::string to_string() override;
        void evaluate(TypeChecker *ptr) override;
        void setConst() override;
    public:
};

struct ErrorType : Type {
    ErrorType();
    bool equals(Type* other) override;
    std::string to_string() override;
    void evaluate(TypeChecker *ptr) override;
    void setConst() override;  
};

struct ArrayType : public Type {
    ArrayType();
    void setBaseTypeAndReverse(TypePtr ptr);
    void addDim(expPtr p);
    // evaluate all pending indexes into integers
    void evaluate(TypeChecker *ptr);
    std::string to_string() override;
    void printUnevaludatedType(std::string prefix, std::string info_prefix);
    bool equals(Type* other) override;
    void setConst();
    TypeKind kind;
    std::vector<expPtr> pendingDims;
    std::vector<size_t> dims;
    TypePtr element_type;
};

struct FuncType : public Type {
    FuncType();
    void setRetTypeAndReverse(TypePtr ptr);
    void setRetType(TypePtr ptr);
    void addArgType(TypePtr argType);
    void setConst() override;
    bool equals(Type *other);
    std::string to_string() override;
    void evaluate(TypeChecker *ptr) override;
    TypeKind kind = TypeKind::Function;
    TypePtr retType = nullptr;
    std::vector<TypePtr> argTypeList;
};

using FuncTypePtr = std::unique_ptr<FuncType>;

enum AccessSpecifier {
    PUBLIC, PRIVATE
};



// Field & Method Metadata
struct FieldInfo {
    std::string name;
    TypePtr type;
    AccessSpecifier access;
};

struct MethodInfo {
    std::string name;
    TypePtr type;
    bool isVirtual;
    bool isOverride;    // Marks if it overrides a base method
    AccessSpecifier access;
};

struct ClassType : Type {
    ClassType(std::string name, std::string base);
    std::string to_string() override;
    bool equals(Type* other) override;
    void evaluate(TypeChecker *ptr) override;
    void setConst() override;
    std::string name;
    std::string baseClass;                  // Single parent (empty if none)
    // The class scope
    std::unordered_map<std::string, FieldInfo> fields;          // Member variables
    std::unordered_map<std::string, MethodInfo> methods;        // Member functions
    TypeKind kind = TypeKind::Class;
    bool isPolymorphic = false;             // Has virtual methods?
};


struct TypeFactory {
    public:
        static TypePtr getTypeFromName(const std::string& type_name);
        static TypePtr getVoid();
        static TypePtr getInt();
        static TypePtr getFloat(); 
        static TypePtr getChar();
        static TypePtr getBool();
        static TypePtr getArray();
    private:
        static TypePtr makePrimitive(TypeKind kind, size_t size);
};

#endif