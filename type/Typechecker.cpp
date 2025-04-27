#include "parser/astnodes/node.hpp"
#include "symbolTable/symbolTable.hpp"
#include "types/TypeChecker.hpp"
const analyzeInfo HASERROR = analyzeInfo {
    .type = new ErrorType(),
};

void TypeChecker::TypeError(node *ptr, const std::string &str) {
    if(ptr->error_msg == "") {
        ptr->error_msg = color::red + std::string(" error: ") + color::reset + str;
        std::stringstream ss;
        if(ptr->location.first - 1 < source.size()) {
            ss  << "\n"
                << source[ptr->location.first - 1] // \n is already in the row
                << std::string(ptr->location.second - 1, ' ') + color::green << "^" << color::reset;
        } else { //reporting errors
            std::cout << "TypeError Fault! The location is illegal\n";
            exit(1);
        }
        const auto &postfix = ss.str();
        ss.str("");
        ss  << color::bold_black
            <<  ":"  
            <<  ptr->location.first << ":"
            <<  ptr->location.second << ":"
            <<  color::reset;
        errorMessages.push_back(ss.str() + ptr->error_msg + postfix);
    }
}

TypeChecker::TypeChecker()
{
    symbolTable = new SymbolTable();
    loopDepth = 0;
    currentClassDef = nullptr;
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

    if(symbolTable->isInGlobal(node->name)) {
        std::stringstream ss;
        ss  << "Class '"
            << node->name
            << "' is redefined in the global scope";
        TypeError(node, ss.str());
        return HASERROR;
    }

    if(node->name == "this") {
        TypeError(node, "'this' cannot be used as a class name");
    }

    symbolTable->insert(node->name, Symbol{.kind = symbolKind::CLASS_DEF,
                                        .type = class_ptr,
                                        .data = nullptr});
    node->type = class_ptr;

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
                        .type = p->defs[i]->type,
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
        if(node->children[i]->kind == ASTKind::Func_Def) {
            func_def *p = dynamic_cast<func_def*>(node->children[i].get());
            analyzeFunctionBody(p);
        } else if(node->children[i]->kind == ASTKind::Var_Decl) {
            var_decl *p = dynamic_cast<var_decl*>(node->children[i].get());
            for(size_t i = 0; i < p->defs.size(); i ++) {
                analyzeInit(p->defs[i].get());
            }
        }
    }
    std::cout << "class scope:\n";
    symbolTable->printCurScope();
    symbolTable->endScope();
    currentClassDef = nullptr;
    return analyzeInfo();
}

// ========================
// Expression Nodes
// ========================
void TypeChecker::analyzeAdd(binary_expr *node) {
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = node->left->inferred_type;
    } else if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = node->left->inferred_type;
    } else if(node->right->inferred_type->kind == TypeKind::Pointer && node->left->inferred_type->kind == TypeKind::Int) {
        auto ptr = std::move(node->left);
        node->left = std::move(node->right);
        node->right = std::move(ptr);
        node->inferred_type = node->left->inferred_type;
    } else if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Pointer && node->op == "-") {
        node->inferred_type = TypeFactory::getInt();
    } 
}

void TypeChecker::analyzeMul(binary_expr *node) {
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = TypeFactory::getInt();
    }
}

void TypeChecker::analyzeBitAnd(binary_expr *node) {
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = TypeFactory::getInt();
    } 
}

void TypeChecker::analyzeLogicAnd(binary_expr *node) {
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = TypeFactory::getInt();
    }
}

void TypeChecker::analyzeCompare(binary_expr *node) {
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = TypeFactory::getInt();
    }
    if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Pointer) {
        if(node->left->inferred_type->equals(node->right->inferred_type)) {
            node->inferred_type = TypeFactory::getInt();
        }
    }
}

void TypeChecker::analyzeEq(binary_expr *node) {
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = TypeFactory::getInt();
    } 
}

analyzeInfo TypeChecker::analyze(binary_expr* node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    if(info1.type == nullptr || info2.type == nullptr) {
        std::cout << "binary_expr analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    if(info1.type->kind == TypeKind::Array) {
        ArrayType *array = dynamic_cast<ArrayType*>(info1.type);
        Type *degrade_array = array->degrade();
        type_cast *tc = new type_cast(std::move(node->left), degrade_array);
        tc->inferred_type = degrade_array;
        node->left = expPtr(tc);
        if(array->dims.size() > 1) {
            std::stringstream ss;
            ss << "Operator '" << node->op << "' cannot apply on type " 
            << degrade_array->to_string() 
            << " and " << node->right->inferred_type->to_string() << "";
            TypeError(node, ss.str());
            node->inferred_type = HASERROR.type;
        } else {
            node->inferred_type = degrade_array;
        }
    }
    if(info2.type->kind == TypeKind::Array) {
        ArrayType *array = dynamic_cast<ArrayType*>(info2.type);
        Type *degrade_array = array->degrade();
        type_cast *tc = new type_cast(std::move(node->right), degrade_array);
        tc->inferred_type = degrade_array;
        node->right = expPtr(tc);
        if(array->dims.size() > 1) {
            std::stringstream ss;
            ss << "Operator '" << node->op << "' cannot apply on type " 
            << node->left->inferred_type->to_string() 
            << " and " << degrade_array->to_string() << "";
            TypeError(node, ss.str());
            node->inferred_type = HASERROR.type;
        } else {
            node->inferred_type = degrade_array;
        }
    }
    if(node->op == "+" || node->op == "-") {
        analyzeAdd(node);
    }
    if(node->op == "*" || node->op == "/") {
        analyzeMul(node);
    }
    if(node->op == "&" || node->op == "|") {
        analyzeBitAnd(node);
    }
    if(node->op == "&&" || node->op == "||") {
        analyzeLogicAnd(node);
    }
    if(node->op == ">" || node->op == "<") {
        analyzeCompare(node);
    }
    if(node->op == "==" || node->op == "!=") {
        analyzeEq(node);
    }
    if(node->op == "<=" || node->op == ">=") {

    }
    if(node->inferred_type == nullptr) {
        node->inferred_type = HASERROR.type;
        std::stringstream ss;
        ss << "Operator '" << node->op << "' cannot apply on type " 
        << info1.type->to_string() 
        << " and " << info2.type->to_string() << "";
        TypeError(node, ss.str());
    }
    return analyzeInfo{
        .type = node->inferred_type
    };
}

/*
This function have to be called after operand is analyzed.
node->inferred_type will be computed.
*/
void TypeChecker::analyzeDeref(unary_expr *node) {
    auto info = analyzeInfo{
        .type = node->operand->inferred_type
    };
    if(info.type->kind == TypeKind::Array) {
        ArrayType *array = dynamic_cast<ArrayType*>(info.type);
        Type *degrade_array = array->degrade();
        type_cast *tc = new type_cast(std::move(node->operand), degrade_array);
        tc->inferred_type = degrade_array;
        node->operand = expPtr(tc);
        info.type = degrade_array;
    }
    if(info.type->kind != TypeKind::Pointer) {
        TypeError(node, "Type '" + info.type->to_string() + "' is not a pointer type");
        node->inferred_type = HASERROR.type;
        return;
    }
    PointerType *pointer = dynamic_cast<PointerType*>(info.type);
    if(pointer->depth == 1) {
        node->inferred_type = pointer->elementType;
    } else {
        PointerType *new_pointer = new PointerType();
        new_pointer->depth = pointer->depth - 1;
        new_pointer->elementType = pointer->elementType;
        node->inferred_type = static_cast<Type*>(new_pointer);
    }
}


analyzeInfo TypeChecker::analyze(unary_expr* node) {
    auto info = node->operand->dispatch(this);
    if(info.type == nullptr) {
        std::cout << "unary_expr analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    if(node->op == "*") {
        analyzeDeref(node);
        return analyzeInfo{
            .type = node->inferred_type
        };
    }
    if(!info.type->equals(TypeFactory::getInt())) {
        std::stringstream ss;
        ss << "Operator " << node->op << " cannot apply on type " 
           << info.type->to_string() << "";
        TypeError(node, ss.str());
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    node->inferred_type = TypeFactory::getInt();
    return analyzeInfo{
        .type = node->inferred_type
    };
}

analyzeInfo TypeChecker::analyze(subscript_expr *node)
{
    auto info1 = node->list->dispatch(this);
    auto info2 = node->sub->dispatch(this);

    if (info1.type->equals(HASERROR.type) || info2.type->equals(HASERROR.type)) {
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }

    if (!info2.type->equals(TypeFactory::getInt())) {
        TypeError(node->sub.get(), "Array subscript must be of integer type");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }

    //set base_type for codeGen
    node->base_type = info1.type;
    
    if (node->base_type->kind == TypeKind::Pointer) {
        PointerType *ptr = dynamic_cast<PointerType*>(node->base_type);
        if (ptr->depth == 1) {
            node->inferred_type = ptr->elementType;
            return analyzeInfo{.type = node->inferred_type};
        } else {
            PointerType *new_ptr = TypeFactory::getPointer();
            new_ptr->elementType = ptr->elementType;
            new_ptr->depth = ptr->depth - 1;
            node->inferred_type = new_ptr;
            return analyzeInfo{.type = node->inferred_type};
        }
    }

    // Handle array types
    if (node->base_type->kind == TypeKind::Array) {
        ArrayType *arr = dynamic_cast<ArrayType*>(node->base_type);
        if (arr->dims.size() == 1) {
            node->inferred_type = arr->element_type;
            return analyzeInfo{.type = node->inferred_type};
        } else {
            ArrayType *new_arr = dynamic_cast<ArrayType*>(TypeFactory::getArray());
            new_arr->element_type = arr->element_type;
            for (size_t i = 1; i < arr->dims.size(); i++) {
                new_arr->dims.push_back(arr->dims[i]);
            }
            node->inferred_type = new_arr;
            return analyzeInfo{.type = node->inferred_type};
        }
    }

    node->inferred_type = HASERROR.type;
    return HASERROR;
}

analyzeInfo TypeChecker::analyze(identifier *node)
{
    if(!symbolTable->exists(node->name)) {
    TypeError(node, "Identifer '" + node->name + "' is not bound");
    node->inferred_type = HASERROR.type;
    return HASERROR;
    } 
    auto sym = symbolTable->getValue(node->name);
    if(sym.type->equals(HASERROR.type)) {
        TypeError(node, "Identifer '" + node->name + "' is ill-typed");
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
    node->inferred_type = sym.type;
    return analyzeInfo{
        .type = node->inferred_type
    };
}

// analyzeInfo TypeChecker::analyze(lval_expr* node) {
//     if(!symbolTable->exists(node->id)) {
//         TypeError(node, "Identifer '" + node->id + "' is not bound");
//         node->inferred_type = HASERROR.type;
//         return HASERROR;
//     } 
//     auto sym = symbolTable->getValue(node->id);
//     if(sym.type->equals(HASERROR.type)) {
//         TypeError(node, "Identifer '" + node->id + "' is ill-typed");
//         node->inferred_type = HASERROR.type;
//         return HASERROR;
//     }
//     if(sym.kind == symbolKind::FUNCTION || sym.type->kind == TypeKind::Function) {
//         TypeError(node, "Lval cannot be function");
//         node->inferred_type = HASERROR.type;
//         return HASERROR;
//     }
//     if(sym.kind == symbolKind::CLASS_DEF) {
//         TypeError(node, "Lval cannot be classname");
//         node->inferred_type = HASERROR.type;
//         return HASERROR;
//     }
//     node->id_type = sym.type;

//     size_t rem_dim = node->dims.size();
//     if(rem_dim == 0) {
//         node->inferred_type = sym.type;
//         return analyzeInfo{
//             .type = sym.type
//         };
//     }
//     Type *cur = sym.type;
//     size_t cur_dim = 0;
//     if(cur->kind == TypeKind::Array) {
//         cur_dim = dynamic_cast<ArrayType*>(cur)->dims.size();
//     } else if(cur->kind == TypeKind::Pointer) {
//         cur_dim = dynamic_cast<PointerType*>(cur)->depth;
//     }
//     while(rem_dim != 0) {
//         if(cur_dim > rem_dim) {
//             if(cur->kind == TypeKind::Pointer) {
//                 PointerType *pointer = dynamic_cast<PointerType*>(cur);
//                 PointerType *new_pointer = TypeFactory::getPointer();
//                 new_pointer->elementType = pointer->elementType;
//                 new_pointer->depth = cur_dim - rem_dim;
//                 node->inferred_type = new_pointer;
//             } else if(cur->kind == TypeKind::Array) {
//                 ArrayType *arr = dynamic_cast<ArrayType*>(cur);
//                 ArrayType *new_arr = dynamic_cast<ArrayType*>(TypeFactory::getArray());
//                 new_arr->element_type = arr->element_type;
//                 for(size_t i = 0; i < cur_dim - rem_dim; i++) {
//                     new_arr->dims.push_back(arr->dims[rem_dim + i]);
//                 }
//                 node->inferred_type = new_arr;
//             }
//             return analyzeInfo{
//                 .type = node->inferred_type
//             };
//         } else if(cur_dim == rem_dim) {
//             rem_dim -= cur_dim;
//             if(cur->kind == TypeKind::Pointer) {
//                 PointerType *pointer = dynamic_cast<PointerType*>(cur);
//                 node->inferred_type = pointer->elementType;
//                 return analyzeInfo{
//                     .type = node->inferred_type
//                 };
//             } else if(cur->kind == TypeKind::Array) {
//                 ArrayType *array = dynamic_cast<ArrayType*>(cur);
//                 node->inferred_type = array->element_type;
//                 return analyzeInfo{
//                     .type = node->inferred_type
//                 };
//             }
//         } else {
//             rem_dim -= cur_dim;
//             if(cur->kind == TypeKind::Pointer) {
//                 PointerType *pointer = dynamic_cast<PointerType*>(cur);
//                 if(pointer->elementType->kind == TypeKind::Array) {
//                     cur_dim = dynamic_cast<ArrayType*>(pointer->elementType)->dims.size();
//                 } else if(pointer->elementType->kind == TypeKind::Pointer) {
//                     std::cout << "In analyze(lval_expr*), the pointer cannot point to a pointer\n";
//                     exit(1);
//                 } else {
//                     cur_dim = 0; //primitives or class
//                 }
//                 cur = pointer->elementType;
//             } else if(cur->kind == TypeKind::Array) {
//                 ArrayType *array = dynamic_cast<ArrayType*>(cur);
//                 if(array->element_type->kind == TypeKind::Pointer) {
//                     cur_dim = dynamic_cast<PointerType*>(array->element_type)->depth;
//                 } else if(array->element_type->kind == TypeKind::Array) {
//                     std::cout << "In analyze(lval_expr*), the array cannot hold another array\n";
//                     exit(1);
//                 } else {
//                     cur_dim = 0;
//                 }
//                 cur = array->element_type;
//             } else {
//                 TypeError(node, "Indexes mismatch");
//                 node->inferred_type = HASERROR.type;
//                 return HASERROR;
//             }
//         }
//     }


//     node->inferred_type = sym.type;
//     return analyzeInfo{
//         .type = sym.type
//     };
// }


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
        if(node->args[i]->inferred_type == nullptr) {
            std::cout << "fun_call analyze fail, the fault is at:\n";
            node->printAST("","");
            exit(1);
        }
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
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    for(size_t i = 0; i < std::min(node->args.size(), type->argTypeList.size()); i++) {
        if(!type->argTypeList[i]->equals(node->args[i]->inferred_type)) {
            std::stringstream ss;
            ss  << "The " << i << "th argument of function '" << node->func_name 
            << "' is expected to have type "
            << type->argTypeList[i]->to_string() << ", but "
            << node->args[i]->inferred_type->to_string() << " is provided";
            TypeError(node, ss.str());
            node->inferred_type = HASERROR.type;
            return HASERROR;
        }
    }

    node->inferred_type = type->retType;
    return analyzeInfo{
        .type = node->inferred_type
    };
}

// ========================
// Literal Nodes
// ========================

analyzeInfo TypeChecker::analyze(int_literal* node) {
    node->inferred_type = TypeFactory::getInt();
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
    if(info1.type == nullptr) {
        std::cout << "if_else_stmt analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    loopDepth++;
    if(!info1.type->equals(TypeFactory::getInt())) {
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
    if(info1.type == nullptr) {
        std::cout << "while stmt analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    if(!info1.type->equals(TypeFactory::getInt())) {
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
        if(!currentFuncDef->is_constructor && !currentFuncDef->type->retType->equals(TypeFactory::getVoid())) {
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
    if(info.type == nullptr) {
        std::cout << "return_stmt analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    if(currentFuncDef->is_constructor) {
        TypeError(node, "The constructor shouldn't return any value");
    } else if(!currentFuncDef->type->retType->equals(info.type)) {
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
    if(currentClassDef != 0) {
        node->type->bindings.insert(node->type->bindings.begin(), "this");
        ClassVarType *cv = TypeFactory::getClassVar(currentClassDef->name);
        PointerType *pointer = TypeFactory::getPointer();
        pointer->elementType = cv;
        node->type->argTypeList.insert(node->type->argTypeList.begin(), pointer);
    }
    for(size_t i = 0; i < node->type->argTypeList.size(); i++) {
        if(symbolTable->isInCurrentScope(node->type->bindings[i])) {
            std::stringstream ss;
            ss  << "'" + node->type->bindings[i] + "'"
                << "is redefined\n";
            TypeError(node, ss.str());
        }
        Type *tmp = node->type->argTypeList[i];
        if(tmp->kind == TypeKind::Array) {
            ArrayType *arr = dynamic_cast<ArrayType*>(tmp);
            node->type->argTypeList[i] = arr->degrade();
        }
        symbolTable->insert(node->type->bindings[i], 
            Symbol{
                .kind = VARIABLE,
                .type = node->type->argTypeList[i]
            });
    }
    
    bool hasReturnStmt = false;
    for(size_t i = 0; i < node->body.size(); i++) {
        node->body[i]->printAST("","");
        node->body[i]->dispatch(this);
        if(node->body[i]->kind == ASTKind::Return_Stmt) {
            return_stmt *rt = dynamic_cast<return_stmt*>(node->body[i].get());
            if(rt->value) hasReturnStmt = true;
        }
    }
    if(node->is_constructor || node->type->retType->equals(TypeFactory::getVoid())) {
        if(hasReturnStmt) {
            TypeError(node, "The constructor shouldn't return any value"); 
        }
    } else if(!node->type->retType->equals(TypeFactory::getVoid())) {
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
    node->type->evaluate(this);

    // symbolTable->printCurScope();
    if(node->is_constructor && node->name != currentClassDef->name) {
        TypeError(node, "The class constructor '" + node->name + "' must be of the same name as the class");
    }
    if(node->name == "this") {
        TypeError(node, "'this' cannot be used as a function name");
    }
    if(symbolTable->isInCurrentScope(node->name)) {
        std::stringstream ss;
        ss << "Function redefined in the global scope";
        TypeError(node, ss.str());
        return HASERROR;
    }

    symbolTable->insert(node->name, Symbol{.kind = FUNCTION,
                                           .type = node->type,
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
            if(child->scalar->inferred_type == nullptr) {
                std::cout << "init analyze fail, the fault is at:\n";
                ptr->printAST("","");
                exit(1);
            }
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
        auto info = new_child->scalar->dispatch(tc);
        if(info.type == nullptr) {
            std::cout << "init analyze fail, the fault is at:\n";
            new_child->scalar->printAST("","");
            exit(1);
        }
        result->addChild(initValPtr(new_child));
    }
    return {result, SUCCESS};
}

void TypeChecker::analyzeInit(var_def* node) {
    if(node->init_val) {
        init_val *p = static_cast<init_val*>(node->init_val.get());
        if(!(node->type->kind == TypeKind::Array)) {
            if(p->scalar == nullptr) {
                std::stringstream ss;
                ss << "scalar can not be initialzed with a list";
                TypeError(node, ss.str());
            } else {
                auto info = p->scalar->dispatch(this);
                if(info.type == nullptr) {
                    std::cout << "var_def analyze fail, the fault is at:\n";
                    node->printAST("","");
                    exit(1);
                }
                if(!info.type->equals(node->type)) {
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
                ArrayType *type = dynamic_cast<ArrayType*>(node->type);
                auto pair = normalization(p, type->dims, type->element_type, this);
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
    if(node->id == "this") {
        TypeError(node, "'this' cannot be explicitly declared as assignable");
        return analyzeInfo();
    }
    if(node->type->equals(TypeFactory::getVoid())) {
        std::cout << "Invalid use of type 'void' in variable declaration at (" << node->location.first 
        << "," << node->location.second << ")" << std::endl;
    }
    node->type->evaluate(this);
    if(node->type->kind == TypeKind::Array) {
        for(auto dim : dynamic_cast<ArrayType*>(node->type)->dims) {
            if(dim <= 0) {
                TypeError(node, "Invalid index in array declaration");
                return HASERROR;
            }
        }
    }
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

    if(node->type)

    symbolTable->insert(node->id, 
    Symbol{
        .kind = symbolKind::VARIABLE,
        .type = node->type,
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
    if(info.type == nullptr) {
        std::cout << "member_access analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    if(node->exp->inferred_type->kind != TypeKind::ClassVar) {
        TypeError(node, "The expression at the left of the dot should be an object, but its type is '" + node->exp->inferred_type->to_string() + "'");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    ClassVarType *classType = dynamic_cast<ClassVarType*>(node->exp->inferred_type);
    if(!symbolTable->isInGlobal(classType->classname)) {
        TypeError(node, "The classname '" + classType->classname + "' is not bound");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    
    auto sym = symbolTable->getFromGlobal(classType->classname);
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
            if(node->name == classType->classname) {
                TypeError(node, "Constructor cannot be called using pointer");
                node->inferred_type = HASERROR.type;
                return analyzeInfo{
                    .type = node->inferred_type
                };
            }
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
                auto info1 = node->args[i]->dispatch(this);
                if(info1.type == nullptr) {
                    std::cout << "member_access analyze fail, the fault is at:\n";
                    node->printAST("","");
                    exit(1);
                }
                if(!type->argTypeList[i]->equals(node->args[i]->inferred_type)) {
                    std::stringstream ss;
                    ss  << "The " << i << "th argument of function '" << node->name 
                    << "' is expected to have type "
                    << type->argTypeList[i]->to_string() << ", but "
                    << node->args[i]->inferred_type->to_string() << " is provided";
                    TypeError(node, ss.str());
                }
            }
            node->inferred_type = type->retType;
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

analyzeInfo TypeChecker::analyze(pointer_acc *node) {
    auto info = node->exp->dispatch(this);
    if(info.type == nullptr) {
        std::cout << "point_acc analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    if(node->exp->inferred_type->kind != TypeKind::Pointer) {
        TypeError(node, "The expression at the left of the dot should be a pointer to an object, but its type is '" + node->exp->inferred_type->to_string() + "'");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    PointerType *pointer = dynamic_cast<PointerType*>(node->exp->inferred_type);
    if(pointer->elementType == nullptr) {
        std::cout << "In analyze pointer_acc, pointer->elementtype is nullpointer\n";
        exit(1);
    }
    if(!(pointer->elementType->kind == TypeKind::ClassVar)) {
        TypeError(node, "The expression at the left of the dot should be a pointer to an object, but its type is '" + node->exp->inferred_type->to_string() + "'");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }

    ClassVarType *classType = dynamic_cast<ClassVarType*>(pointer->elementType);
    if(!symbolTable->isInGlobal(classType->classname)) {
        TypeError(node, "The classname '" + classType->classname + "' is not bound");
        node->inferred_type = HASERROR.type;
        return HASERROR;
    }
    auto sym = symbolTable->getFromGlobal(classType->classname);
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
            std::cout << classType->classname << "\n";
            if(node->name == classType->classname) {
                TypeError(node, "Constructor cannot be called using pointer");
                node->inferred_type = HASERROR.type;
                return analyzeInfo{
                    .type = node->inferred_type
                };
            }
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
                auto info2 = node->args[i]->dispatch(this);
                if(info2.type == nullptr) {
                    std::cout << "pointer_acc analyze fail, the fault is at:\n";
                    node->printAST("","");
                    exit(1);
                }
                if(!type->argTypeList[i]->equals(node->args[i]->inferred_type)) {
                    std::stringstream ss;
                    ss  << "The " << i << "th argument of function '" << node->name 
                    << "' is expected to have type "
                    << type->argTypeList[i]->to_string() << ", but "
                    << node->args[i]->inferred_type->to_string() << " is provided";
                    TypeError(node, ss.str());
                }
            }
            node->inferred_type = type->retType;
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
            if(info1.type == nullptr) {
                std::cout << "array_type analyze fail, the fault is at:\n";
                node->printUnevaludatedType("","");
                exit(1);
            }
            if(!info1.type->equals(TypeFactory::getInt()) && !node->hasError) {
                node->hasError = true;
                std::stringstream ss;
                ss << "The " << i << "th index of array type is not of integer type";
                node->errorMsgs.push_back(ss.str());
            }
            constInfo info = p->const_eval(this);
            if(info.is_const) {
                if(info.type->equals(TypeFactory::getInt())) {
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

// constInfo TypeChecker::const_eval(lval_expr *node)
// {
//     if(!symbolTable->exists(node->id)) {
//         TypeError(node, "The symbol name is not found");
//         return constInfo();
//     } 
//     Symbol sym = symbolTable->getValue(node->id);
//     if(node->inferred_type->is_const == false) {
//         TypeError(node, "The bound value is not known at compile time");
//         return constInfo();
//     } else if(node->inferred_type->equals(TypeFactory::getInt())) {
//         if(node->dims.size() == 0) {
//             return constInfo{
//                 .is_const = true,
//                 .value = sym.data,
//                 .type = node->inferred_type
//             };
//         }
//     } else if(node->inferred_type->kind == TypeKind::Array) {
        
//     }
//     return constInfo();
// }

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
    node->inferred_type = TypeFactory::getInt();
    return constInfo{
        .is_const = true,
        .value = (void*)(new int(node->value)),
        .type = node->inferred_type
    };
}

void TypeChecker::dumpErrors(std::string path) {
    for(const auto &str : errorMessages) {
        std::cout << color::bold_black << path << color::reset << str << std::endl;
    }
}

bool TypeChecker::hasTypeError() {
    return errorMessages.size() > 0;
}