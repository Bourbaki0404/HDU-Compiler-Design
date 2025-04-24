#include "parser/astnodes/node.hpp"

codeGen::codeGen() {
    moduleInit();
    setOutputFileName("out.ll");
}

void codeGen::setOutputFileName(const std::string &name) {
    outFileName = name;
} 

codeGenInfo codeGen::analyzeAdd(binary_expr *node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    llvm::Value *result;
    if(node->left->inferred_type->kind == TypeKind::Int && node->left->inferred_type->kind == TypeKind::Int) {
        if(node->op == "+") result = builder->CreateAdd(info1.value, info2.value);
        if(node->op == "-") result = builder->CreateSub(info1.value, info2.value);
    } else if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Int) {
        result = builder->CreateGEP(to_llvm_type(node->left->inferred_type), info1.value, info2.value);
    } else if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Pointer && node->op == "-") {
        // builder->CreatePtrDiff(to_llvm_type(node->left->inferred_type) , info1.value, info2.value);
    } 
    return codeGenInfo{
        .value = result
    };
}

codeGenInfo codeGen::analyzeMul(binary_expr *node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    llvm::Value *result;
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        if(node->op == "*") result = builder->CreateMul(info1.value, info2.value);
        if(node->op == "/") result = builder->CreateSDiv(info1.value, info2.value);
    }
    return codeGenInfo{
        .value = result
    };
}

codeGenInfo codeGen::analyzeCompare(binary_expr *node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    llvm::Value *result;
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        if(node->op == ">") result = builder->CreateICmpSGT(info1.value, info2.value);
        if(node->op == "<") result = builder->CreateICmpSLT(info1.value, info2.value);
    }
    if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Pointer) {
        if(node->op == ">") result = builder->CreateICmpUGT(info1.value, info2.value);
        if(node->op == "<") result = builder->CreateICmpULT(info1.value, info2.value);
    }
    return codeGenInfo{
        .value = result
    };
}

codeGenInfo codeGen::analyzeEq(binary_expr *node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    llvm::Value *result;
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        if(node->op == "==") result = builder->CreateICmpEQ(info1.value, info2.value);
        if(node->op == "!=") result = builder->CreateICmpNE(info1.value, info2.value);
    }
    if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Pointer) {
        if(node->op == "==") result = builder->CreateICmpEQ(info1.value, info2.value);
        if(node->op == "!=") result = builder->CreateICmpNE(info1.value, info2.value);
    }
    return codeGenInfo{
        .value = result
    };
}

codeGenInfo codeGen::analyzeGt(binary_expr *node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    llvm::Value *result;
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        if(node->op == ">=") result = builder->CreateICmpUGT(info1.value, info2.value);
        if(node->op == "<=") result = builder->CreateICmpULT(info1.value, info2.value);
    }
    if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Pointer) {
        if(node->op == ">=") result = builder->CreateICmpUGT(info1.value, info2.value);
        if(node->op == "<=") result = builder->CreateICmpULT(info1.value, info2.value);
    }
    return codeGenInfo{
        .value = result
    };
}

// Expression nodes
codeGenInfo codeGen::analyze(binary_expr* node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    llvm::Value *result;
    if(node->op == "+" || node->op == "-") {
        return analyzeAdd(node);
    }
    if(node->op == "*" || node->op == "/") {
        return analyzeMul(node);
    }
    if(node->op == "&" || node->op == "|") {
        // analyzeBitAnd(node);
    }
    if(node->op == "&&" || node->op == "||") {
        // analyzeLogicAnd(node);
    }
    if(node->op == ">" || node->op == "<") {
        return analyzeCompare(node);
    }
    if(node->op == "==" || node->op == "!=") {
        return analyzeEq(node);
    }
    if(node->op == "<=" || node->op == ">=") {
        return analyzeGt(node);
    }
}

codeGenInfo codeGen::analyze(unary_expr* node) {
    // TODO: Implement unary expression code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(lval_expr* node) {
    if(node->inferred_type->kind == TypeKind::Pointer) {

    } else if(node->inferred_type->kind == TypeKind::Array) {

    } else if(node->inferred_type->kind == TypeKind::ClassVar){

    } else { //primitives
        if(node->inferred_type->kind == TypeKind::Int) {
            auto info = cur->lookup(node->id);
            auto localVar = llvm::dyn_cast<llvm::AllocaInst>(info.value);
            llvm::Value *offset = nullptr;
            if(node->dims.size() > 0) {
                for(size_t i = 0; i < node->dims.size(); i++) {
                    auto suffix_value = node->dims[i]->dispatch(this).value;
                    ArrayType *arr = dynamic_cast<ArrayType*>(node->id_type);
                    if(i == 0) {
                        offset = suffix_value;
                    } else {
                        auto v = builder->CreateMul(builder->getInt32(arr->dims[i]), offset);
                        offset = builder->CreateAdd(v, suffix_value);
                    }
                }
                auto addr = builder->CreateGEP(localVar->getType(), localVar, offset);
                auto value = builder->CreateLoad(localVar->getAllocatedType(), localVar, node->id);
                return codeGenInfo{
                    info.value = value
                };
            } else {
                auto value = builder->CreateLoad(localVar->getAllocatedType(), localVar, node->id);
                return codeGenInfo{
                    info.value = value
                };
            }
        }
    }
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(fun_call* node) {
    // TODO: Implement function call code generation
    return codeGenInfo();
}

// Literal nodes
codeGenInfo codeGen::analyze(int_literal* node) {
    return codeGenInfo{
        .value = builder->getInt32(node->value)
    };
}

// Statement nodes
codeGenInfo codeGen::analyze(expr_stmt* node) {
    node->ptr->dispatch(this);
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(if_else_stmt* node) {
    
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(while_stmt* node) {
    // TODO: Implement while statement code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(break_stmt* node) {
    // TODO: Implement break statement code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(continue_stmt* node) {
    // TODO: Implement continue statement code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(return_stmt* node) {
    auto info = node->value->dispatch(this);
    builder->CreateRet(info.value);
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(block_stmt* node) {
    environment *oldtable = cur;
    cur = new environment(cur);

    for(size_t i = 0; i < node->items.size(); i++) {
        node->items[i]->dispatch(this);
    }

    delete cur;
    cur = oldtable;

    return codeGenInfo();
}

// Declaration/definition nodes
codeGenInfo codeGen::analyze(class_def* node) {
    // TODO: Implement class definition code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(func_def* node) {
    auto fn = node->type;
    auto fnType = static_cast<llvm::FunctionType*>(to_llvm_type(node->type));
    currentFn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, node->name, *module);
    global->insert(node->name, 
    env_info{
        .value = currentFn
    });
    size_t argIdx = 0;
    for(auto it = currentFn->arg_begin(); it != currentFn->arg_end(); it++) {
        it->setName(fn->bindings[argIdx]);
        argIdx++;
    }

    auto entry = llvm::BasicBlock::Create(*ctx, "entry", currentFn);
    builder->SetInsertPoint(entry);
    var_builder->SetInsertPoint(entry);

    environment *oldtable = cur;
    cur = new environment(cur);

    argIdx = 0;
    for (auto& arg : currentFn->args()) {
        auto alloca = builder->CreateAlloca(arg.getType(), nullptr, arg.getName());
        builder->CreateStore(&arg, alloca);
        cur->insert(fn->bindings[argIdx], env_info{
            .value = alloca
        });
        argIdx++;
    }

    for(size_t i = 0; i < node->body.size(); i++) {
        node->body[i]->dispatch(this);
    }

    auto value = builder->CreateAdd(builder->getInt32(1), builder->getInt32(2));

    delete cur;
    cur = oldtable;

    llvm::verifyFunction(*currentFn);
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(var_def* node) {
    auto type = to_llvm_type(node->type);


    auto value = var_builder->CreateAlloca(type, nullptr, node->id);

    cur->insert(node->id, env_info{
        value
    });
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(var_decl* node) {
    for(size_t i = 0; i < node->defs.size(); i++) {
        node->defs[i]->dispatch(this);
    }
    return codeGenInfo();
}

// Program node
codeGenInfo codeGen::analyze(program* node) {
    for(size_t i = 0; i < node->children.size(); i++) {
        node->children[i]->dispatch(this);
    }

    bool hasErrors = llvm::verifyModule(*module, &llvm::errs());
    if (hasErrors) {
        llvm::errs() << "Module verification failed!\n";
        exit(1);
    }
    module->print(llvm::outs(), nullptr);
    saveModuleToFile(outFileName);
    return codeGenInfo();
}

// Initialization node
codeGenInfo codeGen::analyze(init_val* node) {
    // TODO: Implement initialization value code generation
    return codeGenInfo();
}

// Member access nodes
codeGenInfo codeGen::analyze(member_access* node) {
    // TODO: Implement member access code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(pointer_acc* node) {
    // TODO: Implement pointer access code generation
    return codeGenInfo();
}

void codeGen::moduleInit() {
    ctx = std::unique_ptr<llvm::LLVMContext>(new llvm::LLVMContext());
    module = std::unique_ptr<llvm::Module>(new llvm::Module("main", *ctx));
    builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*ctx));
    var_builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*ctx));
    global = new environment();
    cur = global;
}

llvm::Type *codeGen::to_llvm_type(Type *ptr)
{
    if(ptr == nullptr) {
        std::cout << "In to_llvm_type, the ptr is nullptr\n";
        exit(1);
    }
    switch (ptr->kind) {
        case TypeKind::Int:
            return llvm::Type::getInt32Ty(*ctx);
        case TypeKind::Bool:
            return llvm::Type::getInt1Ty(*ctx);
        case TypeKind::Char:
            return llvm::Type::getInt8Ty(*ctx);
        case TypeKind::Float:
            return llvm::Type::getFloatTy(*ctx);
        case TypeKind::Void:
            return llvm::Type::getVoidTy(*ctx);
        case TypeKind::Function: {
            auto fn = dynamic_cast<FuncType*>(ptr);
            auto retType = to_llvm_type(fn->retType);
            std::vector<llvm::Type*> argTypes;
            for(auto type : fn->argTypeList) {
                auto llvm_type = to_llvm_type(type);
                argTypes.push_back(llvm_type);
            }
            return llvm::FunctionType::get(retType, argTypes, false);
        }
        case TypeKind::Pointer: {
            PointerType *pointer = static_cast<PointerType*>(ptr);
            llvm::Type *elementType = to_llvm_type(pointer->elementType);
            llvm::Type *type = elementType;
            for(size_t i = 0; i < pointer->depth; i++) {
                type = type->getPointerTo();
            }
            return type;
        }
        case TypeKind::Array: {
            ArrayType *arrayType = dynamic_cast<ArrayType*>(ptr);
            llvm::Type *elementType = to_llvm_type(arrayType->element_type);
            // handling multi-dimensional array by flattening
            
        }
        default:
            std::cout << "Unsupported type in to_llvm_type: " << ptr->to_string() << std::endl;
            exit(1);
    }
}

environment::environment(environment *parent)
{
    this->parent = parent;
}

environment::environment()
{
    parent = nullptr;
}

void environment::insert(std::string name, env_info info)
{
    mapping[name] = info;
}

env_info environment::lookup(std::string name)
{
    if(mapping.find(name) == mapping.end()) {
        if(parent) {
            return parent->lookup(name);
        } else {
            std::cout << "symbol " + name + " cannot be found in the environment\n";
            exit(1);
        }
    } else {
        return mapping[name];
    }
}

bool environment::isExist(std::string name)
{
    if(mapping.find(name) != mapping.end()) {
        return true;
    } 

    if(parent) {
        return parent->isExist(name);
    }

    return false;
}

bool environment::isInCurEnv(std::string name)
{
    return mapping.find(name) != mapping.end();
}
