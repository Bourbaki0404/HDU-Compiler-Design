#include "parser/astnodes/node.hpp"
#include "symbolTable/symbolTable.hpp"
#include "types/TypeChecker.hpp"
const analyzeInfo HASERROR = analyzeInfo {
    .type = new ErrorType(),
};

void TypeChecker::TypeError(node *ptr, const std::string &str) {
    if(ptr->error_msg == "") {
        ptr->error_msg = RED + std::string(" error: ") + RESET + str;
        std::stringstream ss;
        ss  << BOLD_BLACK
            <<  ":"  
            <<  ptr->location.first << ":"
            <<  ptr->location.second << ":"
            <<  RESET;
        errorMessages.push_back(ss.str() + ptr->error_msg);
    }
}

TypeChecker::TypeChecker()
{
    symbolTable = new SymbolTable();
    loopDepth = 0;
}

/*
    1. The constructor shouldn't return any value.
    2. 

*/
analyzeInfo TypeChecker::analyze(class_def* node) {
std::cout << "Entering class: " << node->name << "\n";
    symbolTable->printCurScope();
    ClassType *class_ptr = new ClassType(node->name, "");

    currentClassDef = node;

    if(symbolTable->exists(node->name)) {
        std::stringstream ss;
        ss  << "Class '"
            << node->name
            << "' is redefined in the global scope";
        TypeError(node, ss.str());
        return HASERROR;
    }

    symbolTable->beginScope();

    // adding declarations
    for(size_t i = 0; i < node->children.size(); i++) {
        if(node->children[i]->kind == ASTKind::Func_Def) {
            func_def *p = dynamic_cast<func_def*>(node->children[i].get());
            analyze(p);
            class_ptr->addMethod(p->name, MethodInfo{
                .type = p->type,
                .isVirtual = false,
                .isConstructor = p->is_constructor,
                .access = AccessSpecifier::PUBLIC
            });
        } else if(node->children[i]->kind == ASTKind::Var_Decl) {
                var_decl *p = dynamic_cast<var_decl*>(node->children[i].get());
                for(size_t i = 0; i < p->defs.size(); i ++) {
                    analyze(p->defs[i].get());
                    class_ptr->addField(p->defs[i]->id, FieldInfo{
                        .type = p->defs[i]->type.get(),
                        .access = AccessSpecifier::PUBLIC
                    });
                }
        } else {
            std::cout << node->children[i]->to_string() << "\n";
            while(1);
        }
    }

    // handling bodies
    for(size_t i = 0; i < node->children.size(); i++) {
      if(node->kind == ASTKind::Func_Def) {
        func_def *p = dynamic_cast<func_def*>(node);
        analyzeFunctionBody(p);
      } else if(node->kind == ASTKind::Var_Decl) {
        var_decl *p = dynamic_cast<var_decl*>(node);
        for(size_t i = 0; i < p->defs.size(); i ++) {
            analyzeInit(p->defs[i].get());
        }
      }
    }
    std::cout << "class scope:\n";
    symbolTable->printCurScope();
    symbolTable->endScope();
    symbolTable->insert(node->name, Symbol{.kind = symbolKind::CLASS_DEF,
                                           .type = class_ptr,
                                           .data = nullptr});
    currentFuncDef = nullptr;
    return analyzeInfo();
}

// ========================
// Expression Nodes
// ========================

analyzeInfo TypeChecker::analyze(binary_expr* node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);

    if(info1.type->equals(TypeFactory::getInt().release()) &&
                info2.type->equals(TypeFactory::getInt().release())) {
        std::cout << "TypeCheckPass at (" << node->location.first << "," << node->location.second << ")" << std::endl;
        node->inferred_type = TypeFactory::getInt().release();
        return analyzeInfo{
            .type = node->inferred_type,
        };
    }
    std::stringstream ss;
    ss << "Operator '" << node->op << "' cannot apply on type " 
       << info1.type->to_string() 
       << " and " << info2.type->to_string() << "";
    TypeError(node, ss.str());
    node->inferred_type = HASERROR.type;
    return HASERROR;
}

analyzeInfo TypeChecker::analyze(unary_expr* node) {
    auto info = node->operand->dispatch(this);
    if(!info.type->equals(TypeFactory::getInt().release())) {
        std::stringstream ss;
        ss << "Operator " << node->op << " cannot apply on type " 
           << info.type->to_string() << "";
        TypeError(node, ss.str());
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    node->inferred_type = TypeFactory::getInt().release();
    return analyzeInfo{
        .type = node->inferred_type
    };
}

analyzeInfo TypeChecker::analyze(lval_expr* node) {
    if(!symbolTable->exists(node->id)) {
        TypeError(node, "Identifer '" + node->id + "' is not bound");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    } 
    auto sym = symbolTable->getValue(node->id);
    if(sym.type->equals(HASERROR.type)) {
        TypeError(node, "Identifer '" + node->id + "' is ill-typed");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    if(sym.kind == symbolKind::FUNCTION || sym.type->kind == TypeKind::Function) {
        TypeError(node, "Lval cannot be function");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    if(sym.kind == symbolKind::CLASS_DEF) {
        TypeError(node, "Lval cannot be classname");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    if(sym.type->kind == TypeKind::Array) {
        ArrayType *type = dynamic_cast<ArrayType*>(sym.type);
        if(node->dims.size() > type->dims.size()) {
            TypeError(node, "Indexes mismatch the arrayType");
            node->inferred_type = HASERROR.type;
            return HASERROR;
        }
        for(size_t i = 0; i < node->dims.size(); i++) {
            auto info = node->dims[i]->dispatch(this);
            if(!info.type->equals(TypeFactory::getInt().get())) {
                TypeError(node, "Index is not integer");
                node->inferred_type = HASERROR.type;
                return HASERROR;
            }
        }
        if(node->dims.size() == type->dims.size()) { //primitive type
            node->inferred_type = type->element_type.get();
            return analyzeInfo{
                .type = node->inferred_type
            };
        }
        ArrayType *new_type = new ArrayType();
        new_type->setBaseTypeAndReverse(TypePtr(type->element_type.get()));
        for(size_t i = 0; i < type->dims.size() - node->dims.size(); i++) {
            new_type->dims.push_back(type->dims[i]);
        }
        node->inferred_type = new_type;
        return analyzeInfo{
            .type = node->inferred_type
        };
    } 
    node->inferred_type = sym.type;
    return analyzeInfo{
        .type = sym.type
    };
}


analyzeInfo TypeChecker::analyze(fun_call* node) {
    if(!symbolTable->exists(node->func_name)) {
        std::stringstream ss;
        ss << "Function '" << node->func_name << "' is not bound";
        TypeError(node, ss.str());
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    for(size_t i = 0; i < node->args.size(); i++) {
        node->args[i]->dispatch(this);
    }
    auto sym = symbolTable->getValue(node->func_name);
    if(sym.kind != FUNCTION || sym.type->kind != TypeKind::Function) {
        std::stringstream ss;
        ss  << "'" << node->func_name << "' is of type "
            << sym.type->to_string() << ", which is not a function type";
        TypeError(node, ss.str());
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    FuncType *type = dynamic_cast<FuncType*>(sym.type);
    if(type->argTypeList.size() != node->args.size()) {
        std::stringstream ss;
        ss  << "Function '" << node->func_name << "' has "
            << type->argTypeList.size() << " arguments, but "
            << node->args.size() << " is provided";
        TypeError(node, ss.str());
    }
    for(size_t i = 0; i < std::min(node->args.size(), type->argTypeList.size()); i++) {
        if(!type->argTypeList[i]->equals(node->args[i]->inferred_type)) {
            std::stringstream ss;
            ss  << "The " << i << "th argument of function '" << node->func_name 
            << "' is expected to have type "
            << type->argTypeList[i]->to_string() << ", but "
            << node->args[i]->inferred_type->to_string() << " is provided";
            TypeError(node, ss.str());
        }
    }

    node->inferred_type = type->retType.get();
    return analyzeInfo{
        .type = node->inferred_type
    };
}

// ========================
// Literal Nodes
// ========================

analyzeInfo TypeChecker::analyze(int_literal* node) {
    node->inferred_type = TypeFactory::getInt().release();
    return analyzeInfo{
        .type = node->inferred_type
    };
}

// ========================
// Statement Nodes
// ========================

analyzeInfo TypeChecker::analyze(expr_stmt* node) {
    return node->ptr->dispatch(this);
}

analyzeInfo TypeChecker::analyze(if_else_stmt* node) {
    auto info1 = node->cond->dispatch(this);
    loopDepth++;
    if(!info1.type->equals(TypeFactory::getInt().get())) {
        std::stringstream ss;
        ss << "The condition is of type " << info1.type->to_string() << ", which is not a numeric type";
        TypeError(node, ss.str());
    }
    node->if_branch->dispatch(this);
    if(node->else_branch) node->else_branch->dispatch(this);
    loopDepth--;
    return analyzeInfo();
}

analyzeInfo TypeChecker::analyze(while_stmt* node) {
    auto info1 = node->cond->dispatch(this);
    if(!info1.type->equals(TypeFactory::getInt().get())) {
        std::stringstream ss;
        ss << "The condition is of type " << info1.type->to_string() << ", which is not a numeric type";
        TypeError(node, ss.str());
    }
    loopDepth++;
    node->body->dispatch(this);
    loopDepth--;
    return analyzeInfo();
}

analyzeInfo TypeChecker::analyze(break_stmt* node) {
    if(loopDepth == 0) {
        TypeError(node, "The break should be within loop");
    }
    return analyzeInfo();
}

analyzeInfo TypeChecker::analyze(continue_stmt* node) {
    if(loopDepth == 0) {
        TypeError(node, "The continue should be within loop");
    }
    return analyzeInfo();
}

analyzeInfo TypeChecker::analyze(return_stmt* node) {
    if(node->value == nullptr) {
        if(!currentFuncDef->type->retType->equals(TypeFactory::getVoid().release())) {
            std::stringstream ss;
            ss  << "The return value should be of type "
                << currentFuncDef->type->retType->to_string()
                << " and cannot be empty";
            TypeError(node, ss.str());
            return analyzeInfo();
        }
        std::cout << "Type deduction PASS for Return\n";
        return analyzeInfo();
    }
    auto info = node->value->dispatch(this);
    if(!currentFuncDef->type->retType->equals(info.type)) {
        std::stringstream ss;
        ss << "The return type should be " << currentFuncDef->type->retType->to_string()
           << ", but the actual type is " << info.type->to_string() << "";
        TypeError(node, ss.str());
        return HASERROR;
    }
    std::cout << "Type deduction PASS for Return\n";
    return analyzeInfo();
}

analyzeInfo TypeChecker::analyze(block_stmt* node) {
    symbolTable->beginScope();
    for(size_t i = 0; i < node->items.size(); i++) {
        node->items[i]->dispatch(this);
    }
    symbolTable->endScope();
    return analyzeInfo();
}

// ========================
// Declaration/Definition Nodes
// ========================

void TypeChecker::analyzeFunctionBody(func_def *node) {
    // symbolTable->printCurScope();
    currentFuncDef = node;
    symbolTable->beginScope();
    for(size_t i = 0; i < node->type->argTypeList.size(); i++) {
        symbolTable->insert(node->type->bindings[i], 
            Symbol{
                .kind = VARIABLE,
                .type = node->type->argTypeList[i].get()
            });
    }
    bool hasReturnStmt = false;
    for(size_t i = 0; i < node->body.size(); i++) {
        node->body[i]->dispatch(this);
        if(node->kind == ASTKind::Return_Stmt) {
            hasReturnStmt = true;
        }
    }
    if(!node->type->retType->equals(TypeFactory::getVoid().get())) {
        if(!hasReturnStmt) {
            std::stringstream ss;
            ss  << "The return value should be of type "
                << currentFuncDef->type->retType->to_string()
                << " and cannot be empty";
            TypeError(node, ss.str());
        }
    }
    // symbolTable->printCurScope();
    symbolTable->endScope();
    currentFuncDef = 0;
}

analyzeInfo TypeChecker::analyze(func_def* node) {
    std::cout << "Entering function: " << node->name << "\n";
    FuncType *p = new FuncType();
    node->type->evaluate(this);
    if(!node->is_constructor) 
        p->setRetType(std::move(node->type->retType));
    else
        p->setRetType(nullptr); //constructor will not return a value

    node->type = p;//SET INFERRED TYPE

    // symbolTable->printCurScope();
    if(node->is_constructor && node->name != currentClassDef->name) {
        TypeError(node, "The class constructor '" + node->name + "' must be of the same name as the class");
    }
    if(symbolTable->isInCurrentScope(node->name)) {
        std::stringstream ss;
        ss << "Function redefined in the global scope";
        TypeError(node, ss.str());
        return HASERROR;
    }

    symbolTable->insert(node->name, Symbol{.kind = FUNCTION,
                                           .type = p,
                                           .data = nullptr});
    return analyzeInfo();
}

// Fill all holes within initialization list with default ctor
enum InitializationStatus {
    SUCCESS,
    NOTALIGNED,
    OVERSIZE,
    TYPEERROR
};

std::pair<init_val*, InitializationStatus> 
normalization(init_val *ptr, std::vector<size_t> dims, Type *elementType, TypeChecker *tc) 
{
    init_val *result = new init_val(std::pair<size_t,size_t>(0, 0));
    size_t pos = 0;

    for(size_t i = 0; i < ptr->children.size(); i++) {
        init_val *child = ptr->children[i].get();
         child->printAST("","");
        if(child->scalar) {
            child->scalar->dispatch(tc); //tc before use inferred type
            if(elementType->equals(child->scalar->inferred_type)) {
                   
                init_val *new_child = new init_val(std::pair<size_t,size_t>(0, 0));
                
                new_child->scalar = std::move(child->scalar);
                
                result->addChild(initValPtr(new_child));
                pos++;
            } else {
                //tc error
                return {result, TYPEERROR};
            }
        } else { //handling list
            if(pos % dims.back() == 0) {
                size_t temp = pos;
                size_t size = 1;
                std::vector<size_t> dims_2;
                for(size_t dim = 0; dim < dims.size() - 1; dim++) {
                    if(temp % dims[dims.size() - dim - 1]) break;
                    size_t length = dims[dims.size() - dim - 1];
                    size *= length;
                    temp /= length;
                    dims_2.push_back(length);
                }
                std::reverse(dims_2.begin(), dims_2.end());
               
                auto pair = normalization(child, dims_2, elementType, tc);
                if(pair.second != SUCCESS) {
                    return {result, pair.second};
                }
                init_val *subarray = pair.first;
                for(auto &item : subarray->children) {
                    result->addChild(std::move(item));
                }
                pos += size;
            } else { //not aligned
                return {result, NOTALIGNED};
            }
        }
    }
    size_t size = 1;
    for(size_t i = 0; i < dims.size(); i++) {
        size *= dims[i];
    }
    std::cout << "SIZE:" << size << "\n";
    if(pos > size) {
        return {result, OVERSIZE};
    }
    for(size_t i = pos; i < size; i++) {
        init_val *new_child = new init_val(std::pair<size_t,size_t>(0, 0));
        int_literal *val = new int_literal(std::pair<size_t,size_t>(0, 0), 0);
        new_child->scalar = expPtr(static_cast<expr*>(val));
        new_child->scalar->dispatch(tc);
        result->addChild(initValPtr(new_child));
    }
    return {result, SUCCESS};
}

void TypeChecker::analyzeInit(var_def* node) {
    if(node->init_val) {
        init_val *p = static_cast<init_val*>(node->init_val.get());
        if(node->type->kind == TypeKind::Int) {
            if(p->scalar == nullptr) {
                std::stringstream ss;
                ss << "scalar can not be initialzed with a list";
                TypeError(node, ss.str());
            } else {
                auto info = p->scalar->dispatch(this);
                if(!info.type->equals(node->type.get())) {
                    std::stringstream ss;
                    ss << "type mismatch in initialization, "
                       << node->type->to_string() << " is not "
                       << info.type->to_string();
                    TypeError(node, ss.str());
                } else {
                    //success
                }
            }
        }
        if(node->type->kind == TypeKind::Array) {
            if(p->scalar != nullptr) {
                std::stringstream ss;
                ss << "scalar can not be used to initialze an array";
                TypeError(node, ss.str());
            } else {
                ArrayType *type = dynamic_cast<ArrayType*>(node->type.get());
                auto pair = normalization(p, type->dims, type->element_type.get(), this);
                switch (pair.second)
                {
                    case NOTALIGNED:
                        TypeError(node, "Initialization list not correctly aligned");
                        break;
                    case OVERSIZE:
                        TypeError(node, "Initialization list is oversized");
                        break;
                    case TYPEERROR:
                        TypeError(node, "Initialization list has type error");
                        break;
                }
                node->init_val = nodePtr(pair.first);
            }
            std::cout << "HD1A\n";
        }
    }
}

analyzeInfo TypeChecker::analyze(var_def* node) {
    if(symbolTable->isInCurrentScope(node->id)) {
        std::stringstream ss;
        ss  << "Variable '"
            << node->id
            <<"'is redefined in the current scope";
        TypeError(node, ss.str());
        return analyzeInfo();
    }
    if(node->type->equals(TypeFactory::getVoid().release())) {
        std::cout << "Invalid use of type 'void' in variable declaration at (" << node->location.first 
        << "," << node->location.second << ")" << std::endl;
    }
    
    node->type->evaluate(this);
    if(node->type->hasError) {
        std::stringstream ss;
        ss  << "The compile-time constant within this type " 
            << node->type->to_string() + " cannot be fully evaluated";
        TypeError(node, ss.str());
        return analyzeInfo();
    }
    // std::cout << "vardef:" + node->id + " " + node->type->to_string() << "\n";
    if(node->is_const) {
        node->type->setConst();
    }

    symbolTable->insert(node->id, 
    Symbol{
        .kind = symbolKind::VARIABLE,
        .type = node->type.get(),
        .data = nullptr
    });
    symbolTable->printCurScope();
    return analyzeInfo();
}

analyzeInfo TypeChecker::analyze(var_decl* node) {
    for(size_t i = 0; i < node->defs.size(); i ++) {
        node->defs[i]->dispatch(this);
    }
    return analyzeInfo();
}

analyzeInfo TypeChecker::analyze(func_param* node) {
    return analyzeInfo();
}

// ========================
// Program Node
// ========================

analyzeInfo TypeChecker::analyze(program* node) {
    for(size_t i = 0; i < node->children.size(); i++) {
        node->children[i]->dispatch(this);
    }
    return analyzeInfo();
}

// ========================
// Initialization Node
// ========================

analyzeInfo TypeChecker::analyze(init_val* node) {
    return analyzeInfo();
}

// In this function the visited node will either represent a field or method, indicating by the isFunc
analyzeInfo TypeChecker::analyze(member_access *node) {
    auto info = node->exp->dispatch(this);
    if(node->exp->inferred_type->kind != TypeKind::ClassVar) {
        TypeError(node, "The expression at the left of the dot should be an object, but its type is '" + node->exp->inferred_type->to_string() + "'");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    ClassVarType *classType = dynamic_cast<ClassVarType*>(node->exp->inferred_type);
    if(!symbolTable->exists(classType->classname)) {
        TypeError(node, "The classname '" + classType->classname + "' is not bound");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    auto sym = symbolTable->getValue(classType->classname);
    if(sym.kind != symbolKind::CLASS_DEF) {
        TypeError(node, "The classname '" + classType->classname + "' is not bound to a class");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    ClassType *classdef = static_cast<ClassType*>(sym.type);
    if(node->isFunc) {
        if(classdef->methods.find(node->name) == classdef->methods.end()) {
            // method not found
            TypeError(node, "The class '" + classType->classname + "' does not have method '" + node->name + "'");
            node->inferred_type = HASERROR.type;
            return HASERROR;
        } else {
            MethodInfo info = classdef->methods[node->name];
            FuncType *type = dynamic_cast<FuncType*>(info.type);
            if(type->argTypeList.size() != node->args.size()) {
                std::stringstream ss;
                ss  << "Function '" << node->name << "' has "
                    << type->argTypeList.size() << " arguments, but "
                    << node->args.size() << " is provided";
                TypeError(node, ss.str());
            }
            for(size_t i = 0; i < std::min(node->args.size(), type->argTypeList.size()); i++) {
                node->args[i]->dispatch(this);
                if(!type->argTypeList[i]->equals(node->args[i]->inferred_type)) {
                    std::stringstream ss;
                    ss  << "The " << i << "th argument of function '" << node->name 
                    << "' is expected to have type "
                    << type->argTypeList[i]->to_string() << ", but "
                    << node->args[i]->inferred_type->to_string() << " is provided";
                    TypeError(node, ss.str());
                }
            }
            node->inferred_type = type->retType.get();
            return analyzeInfo{
                .type = node->inferred_type
            };
        }
    } else {
        if(classdef->fields.find(node->name) == classdef->fields.end()) {
            // field not found
            TypeError(node, "The class '" + classType->classname + "' does not have field '" + node->name + "'");
            node->inferred_type = HASERROR.type;
            return HASERROR;
        }
        auto info = classdef->fields[node->name];
        node->inferred_type = info.type;
        return analyzeInfo{
            .type = info.type
        };
    }
    return HASERROR;
}

analyzeInfo TypeChecker::evaluate(ArrayType *node)
{
    size_t size = node->pendingDims.size();
    for (size_t i = 0; i < size; i++) {
        
        if (node->pendingDims[size - i - 1]) {
            // evaluate pending dims into constants
            expr *p = node->pendingDims[size - i - 1].get();
            auto info1 = p->dispatch(this);//typecheck before const_eval!
            if(!info1.type->equals(TypeFactory::getInt().get()) && !node->hasError) {
                node->hasError = true;
                std::stringstream ss;
                ss << "The " << i << "th index of array type is not of integer type";
                node->errorMsgs.push_back(ss.str());
            }
            constInfo info = p->const_eval(this);
            if(info.is_const) {
                if(info.type->equals(TypeFactory::getInt().release())) {
                    node->dims.push_back(*(int*)info.value);
                }
            } else {
                node->dims.push_back(0);
                if(!node->hasError) {
                    node->hasError = true;
                    std::stringstream ss;
                    ss << "The " << i << "th index of array type is not a compile-time constant integer";
                    node->errorMsgs.push_back(ss.str());
                }
            }

        } else {
            node->dims.push_back(0);
        }
    }
    node->pendingDims.clear();
    return analyzeInfo();
}

analyzeInfo TypeChecker::evaluate(FuncType *node)
{
    for(size_t i = 0; i < node->argTypeList.size(); i++) {
        node->argTypeList[i]->evaluate(this);
    }
    return analyzeInfo();
}

constInfo TypeChecker::const_eval(unary_expr *ptr)
{
    return constInfo();
}

constInfo TypeChecker::const_eval(binary_expr *node)
{
    return constInfo();
}

constInfo TypeChecker::const_eval(lval_expr *node)
{
    if(!symbolTable->exists(node->id)) {
        TypeError(node, "The symbol name is not found");
        return constInfo();
    } 
    Symbol sym = symbolTable->getValue(node->id);
    if(node->inferred_type->is_const == false) {
        TypeError(node, "The bound value is not known at compile time");
        return constInfo();
    } else if(node->inferred_type->equals(TypeFactory::getInt().get())) {
        if(node->dims.size() == 0) {
            return constInfo{
                .is_const = true,
                .value = sym.data,
                .type = node->inferred_type
            };
        }
    } else if(node->inferred_type->kind == TypeKind::Array) {
        
    }
    return constInfo();
}

constInfo TypeChecker::const_eval(fun_call *node)
{
    return constInfo {
        .is_const = false,
        .value = nullptr,
        .type = HASERROR.type
    };
}

//typecheck before const_eval!
constInfo TypeChecker::const_eval(int_literal *node)
{
    node->inferred_type = TypeFactory::getInt().release();
    return constInfo{
        .is_const = true,
        .value = (void*)(new int(node->value)),
        .type = node->inferred_type
    };
}

void TypeChecker::dumpErrors(std::string path) {
    for(const auto &str : errorMessages) {
        std::cout << BOLD_BLACK << path << RESET << str << std::endl;
    }
}