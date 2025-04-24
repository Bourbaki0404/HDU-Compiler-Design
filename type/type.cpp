#include "common/common.hpp"
#include "parser/astnodes/node.hpp"


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

void ArrayType::setBaseTypeAndReverse(Type *ptr) {
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

Type *ArrayType::degrade()
{
    if(dims.size() > 1) {
        PointerType *result = new PointerType();
        ArrayType *array = new ArrayType();
        for(int i = 1; i < dims.size(); i++) {
            array->dims.push_back(dims[i]);
        }
        array->element_type = this->element_type;
        result->elementType = array;
        return result;
    } else {
        if(element_type->kind != TypeKind::Pointer) {
            PointerType *target = new PointerType();
            target->elementType = element_type;
            return target;
        } else {
            PointerType *p = dynamic_cast<PointerType*>(element_type);
            p->depth++;
            return p;
        }
    }
}

void ArrayType::printUnevaludatedType(std::string prefix, std::string info_prefix)
{
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
    if (!element_type->equals(ptr->element_type)) return false;
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

void FuncType::setRetTypeAndReverse(Type *ptr) {
    this->retType = std::move(ptr);
    std::reverse(argTypeList.begin(), argTypeList.end());
}

void FuncType::setConst() {
    is_const = true;
}

void FuncType::setRetType(Type *ptr)
{
    this->retType = std::move(ptr);
}
void FuncType::addArgType(Type *argType)
{
    this->argTypeList.push_back(std::move(argType));
}

bool FuncType::equals(Type *other) {
    if(other == nullptr) return false;
    if(other->kind != TypeKind::Function) return false;
    FuncType *p = static_cast<FuncType*>(other);
    if(!p->retType->equals(this->retType)) return false;
    for(size_t i = 0; i < argTypeList.size(); i++) {
        if(!p->argTypeList[i]->equals(p->argTypeList[i]))
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
    if(elementType == nullptr) return std::string(depth, '*');
    if(elementType->kind == TypeKind::Array) {
        ArrayType *arr = dynamic_cast<ArrayType*>(elementType);
        std::stringstream ss;
        ss  << arr->element_type->to_string()
            << "(*)";
        for(auto dim : arr->dims) {
            ss  <<  "[" << dim << "]";
        }
        return ss.str();
    } else if(elementType->kind == TypeKind::Pointer) { // prohibit pointer from pointing to another pointer
        std::cout << "illegal array type\n";
        exit(1);
    } else {
        return elementType->to_string() + std::string(depth, '*');
    }
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
Type *TypeFactory::getTypeFromName(const std::string& type_name) {
    static const std::unordered_map<std::string, TypeKind> type_map = {
        {"void", TypeKind::Void},
        {"int", TypeKind::Int},
        {"float", TypeKind::Float},
        {"char", TypeKind::Char},
        {"bool", TypeKind::Bool}
    };

    auto it = type_map.find(type_name);
    if (it == type_map.end()) { // classVarType
        return static_cast<Type*>(new ClassVarType(type_name));
    }

    Type *type;
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
            std::cout << "unknown type at getTypeFromName\n";
            exit(1);
    }
    return type;
}

Type *TypeFactory::getVoid() { 
    return makePrimitive(TypeKind::Void, 0); 
}

Type *TypeFactory::getInt() { 
    return makePrimitive(TypeKind::Int, 4); 
}

Type *TypeFactory::getFloat() { 
    return makePrimitive(TypeKind::Float, 4); 
}

Type *TypeFactory::getChar() { 
    return makePrimitive(TypeKind::Char, 1); 
}

Type *TypeFactory::getBool() { 
    return makePrimitive(TypeKind::Bool, 1); 
}

Type *TypeFactory::getArray() {
    typePool.push_back(new ArrayType());
    return typePool.back();
}

ClassVarType *TypeFactory::getClassVar(std::string classname) {
    typePool.push_back(new ClassVarType(classname));
    return dynamic_cast<ClassVarType*>(typePool.back());
}

PointerType *TypeFactory::getPointer() {
    typePool.push_back(new PointerType());
    return dynamic_cast<PointerType*>(typePool.back());
}

PointerType *TypeFactory::getPointerTo(struct Type* ptr) {
    PointerType *p = new PointerType();
    if(ptr->kind == TypeKind::Pointer) {
        PointerType *pointer = dynamic_cast<PointerType*>(ptr);
        p->elementType = pointer->elementType;
        p->depth = pointer->depth + 1;
    } else {
        p->elementType = ptr;
    }
    typePool.push_back(p);
    return dynamic_cast<PointerType*>(typePool.back());
}

FuncType *TypeFactory::getFunction() {
    typePool.push_back(new FuncType());
    return dynamic_cast<FuncType*>(typePool.back());
}

Type *TypeFactory::makePrimitive(TypeKind kind, size_t size) {
    if(primitives.find(kind) == primitives.end()) {
        primitives[kind] = new PrimitiveType(kind, size);
    }
    return primitives[kind];
}

void TypeFactory::deleteAll() {
    for(auto ptr : primitives) {
        delete ptr.second;
    }
    for(auto ptr : typePool) {
        delete ptr;
    }
}

std::map<TypeKind,Type*> TypeFactory::primitives;
std::vector<Type*> TypeFactory::typePool;