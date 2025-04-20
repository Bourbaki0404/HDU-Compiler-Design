#include "common/common.hpp"
#include "parser/astnodes/node.hpp"
#include "types/types.hpp"


// Type
Type::Type(TypeKind kind) 
    : kind(kind), size(0), is_const(false) {}

Type::Type(TypeKind kind, size_t size) 
    : kind(kind), size(size), is_const(false) {}

ErrorType::ErrorType()
: Type(TypeKind::Error) {}

bool ErrorType::equals(Type *other)
{
    return false;
}

std::string ErrorType::to_string()
{
    return "ErrorType";
}
void ErrorType::evaluate(TypeChecker *ptr) {}

void ErrorType::setConst() {}

void PrimitiveType::setConst()
{
    this->is_const = true;
}

// PrimitiveType
PrimitiveType::PrimitiveType(TypeKind kind, size_t size)
    : Type(kind, size) {}

bool PrimitiveType::equals(Type* other) {
    return kind == other->kind;
}

std::string PrimitiveType::to_string() {
    std::string res = is_const ? "const " : "";
    switch (kind) {
        case TypeKind::Void: return "void";
        case TypeKind::Int: return res + "int";
        case TypeKind::Float: return res + "float";
        case TypeKind::Char: return res + "char";
        case TypeKind::Bool: return res + "bool";
        default: return "unknown";
    }
}


void PrimitiveType::evaluate(TypeChecker *ptr) {}

// ArrayType
ArrayType::ArrayType()
    : Type(TypeKind::Array) {
        element_type = nullptr;
    }

void ArrayType::setConst() {
    element_type->setConst();
}

void ArrayType::setBaseTypeAndReverse(TypePtr ptr) {
    element_type = std::move(ptr);
    std::reverse(dims.begin(), dims.end());
}

void ArrayType::addDim(expPtr p) {
    pendingDims.push_back(std::move(p));
}

void ArrayType::evaluate(TypeChecker *ptr) {
    ptr->evaluate(this);
}

std::string ArrayType::to_string() {
    std::string res = element_type ? element_type->to_string() : "";
    if (pendingDims.size() > 0) {
        for (size_t i = 0; i < pendingDims.size(); i++) {
            res += "[?]";
        }
    } else {
        for (size_t i = 0; i < dims.size(); i++) {
            res += "[" + (dims[i] ? std::to_string(dims[i]) : i == 0 ?  "" : "?") + "]";
        }
    }
    return res;
}

void ArrayType::printUnevaludatedType(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << "\n";
    for (size_t i = 0; i < pendingDims.size(); ++i) {
        expr *p = pendingDims[pendingDims.size() - i - 1].get();
        if (i != pendingDims.size() - 1) {
            if(p == nullptr) {
                std::cout << prefix + "├── " << "uncomplete\n";
                continue;
            }
            p->printAST(prefix + "│   ", prefix + "├── ");
        } else {
            if(p == nullptr) {
                std::cout << prefix + "└── " << " uncomplete\n";
                continue;
            }
            p->printAST(prefix + "    ", prefix + "└── ");
        }
    }
}

bool ArrayType::equals(Type* other) {
    if (other == nullptr) return false;
    if (other->kind != TypeKind::Array) return false;
    ArrayType* ptr = static_cast<ArrayType*>(other);
    if (!element_type->equals(ptr->element_type.get())) return false;
    if (dims.size() != ptr->dims.size()) return false;
    for (size_t i = 0; i < dims.size(); i++) {
        if (dims[i] != ptr->dims[i]) return false;
    }
    return true;
}

FuncType::FuncType()
: Type(TypeKind::Function) {
    retType = nullptr;
}

void FuncType::setRetTypeAndReverse(TypePtr ptr) {
    this->retType = std::move(ptr);
    std::reverse(argTypeList.begin(), argTypeList.end());
}

void FuncType::setConst() {
    throw std::runtime_error("funcType can be set to const");
}

void FuncType::setRetType(TypePtr ptr)
{
    this->retType = std::move(ptr);
}
void FuncType::addArgType(TypePtr argType)
{
    this->argTypeList.push_back(std::move(argType));
}

bool FuncType::equals(Type *other) {
    if(other == nullptr) return false;
    if(other->kind != TypeKind::Function) return false;
    FuncType *p = static_cast<FuncType*>(other);
    if(!p->retType->equals(this->retType.get())) return false;
    for(size_t i = 0; i < argTypeList.size(); i++) {
        if(!p->argTypeList[i]->equals(p->argTypeList[i].get()))
            return false;
    }
    return true;
}

void FuncType::evaluate(TypeChecker *ptr = nullptr) {
    ptr->evaluate(this);
    // for(size_t i = 0; i < argTypeList.size(); i++) {
    //     argTypeList[i]->evaluate(ptr);
    // }
}

std::string FuncType::to_string() {
    std::string res = (retType ? retType->to_string() : "") + "(";
    for(size_t i = 0; i < argTypeList.size(); i++) {
        if(i == argTypeList.size() - 1) {
            res += argTypeList[i]->to_string();
        } else {
            res += argTypeList[i]->to_string() + ",";
        }
    }
    res += ")";
    return res;
}

PointerType::PointerType()
: Type(TypeKind::Pointer){
    depth = 1;
    elementType = nullptr;
}

bool PointerType::equals(Type *other) {
    if(other->kind != TypeKind::Pointer) return false;
    auto pointer = dynamic_cast<PointerType*>(other);
    return depth == pointer->depth && elementType->equals(pointer->elementType);
}

void PointerType::setConst() {
    is_const = true;
}

std::string PointerType::to_string() {
    return std::string(depth, '*') + (elementType ? elementType->to_string() : "");
}

void PointerType::evaluate(TypeChecker *ptr) {
}

ClassType::ClassType(std::string name, std::string base) 
: Type(TypeKind::Class) {
    name = name;
    baseClass = base;
}

std::string ClassType::to_string() {
    return {};
}

bool ClassType::equals(Type* other) {
    return false;
}

void ClassType::evaluate(TypeChecker *ptr) {

}

bool ClassType::addMethod(std::string str, MethodInfo info) {
    if(methods.find(str) != methods.end()) {
        return false; //duplication
    }
    methods[str] = info;
    return true;
}

bool ClassType::addField(std::string str, FieldInfo info) {
    if(fields.find(str) != fields.end()) {
        return false; //duplication
    }
    fields[str] = info;
    return true;
}

void ClassType::setConst()
{
    is_const = true;
} 

ClassVarType::ClassVarType(const std::string &classname) 
: Type(TypeKind::ClassVar){
    this->classname = classname;
    is_const = false;
}

bool ClassVarType::equals(Type* other) {
    if(this->kind != other->kind) return false;
    if(this->classname != ((ClassVarType*)other)->classname) return false;
    return true;
}

void ClassVarType::evaluate(TypeChecker *ptr) {
    return;
}

std::string ClassVarType::to_string() {
    return classname;
}

void ClassVarType::setConst() {
    is_const = true;
}

// TypeFactory
TypePtr TypeFactory::getTypeFromName(const std::string& type_name) {
    static const std::unordered_map<std::string, TypeKind> type_map = {
        {"void", TypeKind::Void},
        {"int", TypeKind::Int},
        {"float", TypeKind::Float},
        {"char", TypeKind::Char},
        {"bool", TypeKind::Bool}
    };

    auto it = type_map.find(type_name);
    if (it == type_map.end()) { // classVarType
        return TypePtr(static_cast<Type*>(new ClassVarType(type_name)));
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

TypePtr TypeFactory::getVoid() { 
    return makePrimitive(TypeKind::Void, 0); 
}

TypePtr TypeFactory::getInt() { 
    return makePrimitive(TypeKind::Int, 4); 
}

TypePtr TypeFactory::getFloat() { 
    return makePrimitive(TypeKind::Float, 4); 
}

TypePtr TypeFactory::getChar() { 
    return makePrimitive(TypeKind::Char, 1); 
}

TypePtr TypeFactory::getBool() { 
    return makePrimitive(TypeKind::Bool, 1); 
}

TypePtr TypeFactory::getArray() {
    return std::unique_ptr<ArrayType>(new ArrayType());
}

TypePtr TypeFactory::makePrimitive(TypeKind kind, size_t size) {
    return TypePtr(new PrimitiveType(kind, size));
}