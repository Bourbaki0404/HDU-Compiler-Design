#ifndef __TYPES_H
#define __TYPES_H

#include "common/common.hpp"
#include "types/TypeChecker.hpp"

//forward decl
struct node;
struct program;
struct expr;
struct stmt;
struct block_stmt;
struct func_def;
struct init_val;
using progPtr = std::unique_ptr<program>;
using nodePtr = std::unique_ptr<node>;
using expPtr = std::unique_ptr<expr>;
using stmtPtr = std::unique_ptr<stmt>;
using blockPtr = std::unique_ptr<block_stmt>;
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
    ClassVar,
    Error // when type error happens
};

struct HasBindings {
    std::vector<std::string> bindings;
};


//Don't modify other's type, just creating new types
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
    public:
        std::vector<std::string> errorMsgs;
        bool hasError = false;
};

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
    void setBaseTypeAndReverse(Type *ptr);
    void addDim(expPtr p);
    // evaluate all pending indexes into integers
    void evaluate(TypeChecker *ptr);
    std::string to_string() override;
    Type *degrade();
    void printUnevaludatedType(std::string prefix, std::string info_prefix);
    bool equals(Type* other) override;
    void setConst();
    TypeKind kind;
    std::vector<expPtr> pendingDims;
    std::vector<size_t> dims;
    Type *element_type;
};

struct FuncType : public Type, public HasBindings {
    FuncType();
    void setRetTypeAndReverse(Type *ptr);
    void setRetType(Type *ptr);
    void addArgType(Type *argType);
    void setConst() override;
    bool equals(Type *other);
    std::string to_string() override;
    void evaluate(TypeChecker *ptr) override;
    Type *retType = nullptr;
    std::vector<Type*> argTypeList;
};

using FuncTypePtr = std::unique_ptr<FuncType>;

struct PointerType : public Type {
    PointerType();
    bool equals(Type *other) override;
    void setConst() override;
    std::string to_string() override;
    void evaluate(TypeChecker *ptr) override;
    Type* elementType;
    size_t depth;
};

enum AccessSpecifier {
    PUBLIC, PRIVATE
};



// Field & Method Metadata
struct FieldInfo {
    struct Type *type;
    AccessSpecifier access;
};

struct MethodInfo {
    struct Type *type;
    bool isVirtual;
    bool isConstructor;
    AccessSpecifier access;
};

struct ClassType : Type {
    ClassType(std::string name, std::string base);
    std::string to_string() override;
    bool equals(Type* other) override;
    void evaluate(TypeChecker *ptr) override;
    bool addMethod(std::string str, MethodInfo info);
    bool addField(std::string str, FieldInfo info);
    void setConst() override;
    std::string baseClass;                  // Single parent (empty if none)
    // The class scope
    std::unordered_map<std::string, FieldInfo> fields;          // Member variables
    std::unordered_map<std::string, MethodInfo> methods;        // Member functions
    TypeKind kind = TypeKind::Class;
    bool isPolymorphic = false;             // Has virtual methods?
};

struct ClassVarType : Type {
    ClassVarType(const std::string &classname);
    bool equals(Type* other) override;
    void evaluate(TypeChecker *ptr) override;
    std::string to_string() override;
    void setConst() override;
    std::string classname;
};


struct TypeFactory {
    public:
        static struct Type *getTypeFromName(const std::string& type_name);
        static struct Type *getVoid();
        static struct Type *getInt();
        static struct Type *getFloat(); 
        static struct Type *getChar();
        static struct Type *getBool();
        static struct Type *getArray();
        static ClassVarType *getClassVar(std::string classname);
        static PointerType *getPointer();
        static PointerType *getPointerTo(struct Type *ptr);
        static FuncType *getFunction();
        static void deleteAll();
    private:
        static struct Type* makePrimitive(TypeKind kind, size_t size);
        static std::map<TypeKind,struct Type*> primitives;
        static std::vector<struct Type*> typePool;
};

#endif