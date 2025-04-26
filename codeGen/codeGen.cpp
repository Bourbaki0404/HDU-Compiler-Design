#include "parser/astnodes/node.hpp"

codeGen::codeGen() {
    moduleInit();
    setOutputFileName("out.ll");
    currentFn = nullptr;
    curLoopStart = nullptr;
    curLoopEnd = nullptr;
}

void codeGen::setOutputFileName(const std::string &name) {
    outFileName = name;
} 


/*

Analyze the sum of two values. 
Notice:

To use GEP instruction for pointer arithmic, the type field should be set to the element type. For array
accessing, the type should be the type of the array itself so that the subarray can be extracted.
*/
codeGenInfo codeGen::analyzeAdd(binary_expr *node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    llvm::Value *result;
    llvm::outs() << *info1.value << "\n";
    if(node->left->inferred_type->kind == TypeKind::Int && node->left->inferred_type->kind == TypeKind::Int) {
        if(node->op == "+") result = builder->CreateAdd(info1.value, info2.value);
        if(node->op == "-") result = builder->CreateSub(info1.value, info2.value);
    } else if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Int) {
        llvm::outs() << *info1.value << "\n";
        result = builder->CreateInBoundsGEP(to_llvm_type(node->left->inferred_type)->getPointerElementType(), info1.value, info2.value);
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

/*
Analyze the expression *p, where p could be a pointer to something, or to an array.
If p points to an array, then dereferencing it
*/
codeGenInfo codeGen::analyzePointDeref(unary_expr *node) {
    auto value = node->operand->dispatch(this).value;
    PointerType *pointer = dynamic_cast<PointerType*>(node->operand->inferred_type);
    if(pointer->elementType->kind == TypeKind::Array) {
        llvm::outs() << *value << "\n";
        return codeGenInfo{
            .value = value
        };
    } else { //generate load
        auto valueType = to_llvm_type(node->inferred_type);
        value = builder->CreateLoad(valueType, value);
        llvm::outs() << value << "\n";
        return codeGenInfo{
            .value = value
        };
    }
}

codeGenInfo codeGen::analyzeSimpleUnary(unary_expr *node) {
    auto value = node->operand->dispatch(this).value;
    if(node->op == "-") {
        if(node->inferred_type->kind == TypeKind::Int) {
            value = builder->CreateSub(builder->getInt32(0), value);
        }
    } else if(node->op == "!") {
        if(node->inferred_type->kind == TypeKind::Int) {
            value = builder->CreateZExt(
                builder->CreateICmpEQ(value, builder->getInt32(0)),
                builder->getInt32Ty()
            );
            llvm::outs() << *value->getType() << "\n";
        }
    }
    return codeGenInfo{
        .value = value
    };
}

codeGenInfo codeGen::analyze(unary_expr* node) {
    if(node->op == "*") {
        return analyzePointDeref(node);      
    } else {
        return analyzeSimpleUnary(node);
    }
}

//types except arrayType will generate load
codeGenInfo codeGen::analyze(identifier* node) {
    if(node->inferred_type->kind == TypeKind::Pointer) {
        auto info = cur->lookup(node->name);
        auto localVar = llvm::dyn_cast<llvm::AllocaInst>(info.value);
        auto value = builder->CreateLoad(localVar->getAllocatedType(), localVar, node->name);
        // llvm::outs() << *value << "\n";
        return codeGenInfo{
            info.value = value
        };
    } else if(node->inferred_type->kind == TypeKind::Array) {
        auto info = cur->lookup(node->name);
        return codeGenInfo{.value = info.value};
    } else if(node->inferred_type->kind == TypeKind::ClassVar){

    } else { //primitives
        auto info = cur->lookup(node->name);
        auto localVar = llvm::dyn_cast<llvm::AllocaInst>(info.value);
        auto value = builder->CreateLoad(localVar->getAllocatedType(), localVar, node->name);
        return codeGenInfo{
            info.value = value
        };
    }
}



codeGenInfo codeGen::analyze(subscript_expr *node) {
    auto base_type = to_llvm_type(node->base_type);
    llvm::Type *valueType = nullptr;
    llvm::Value *value = nullptr;
    auto info1 = node->list->dispatch(this);
    auto info2 = node->sub->dispatch(this);
    if(node->base_type->kind == TypeKind::Array) { // distinguish array and pointer to array to enable inbound check of the array
        value = builder->CreateInBoundsGEP(base_type, info1.value, {builder->getInt32(0) , info2.value});
        valueType = base_type->getArrayElementType();
    } else if(node->base_type->kind == TypeKind::Pointer) {
        // llvm::outs() << *info1.value->getType() << "\n";
        // llvm::outs() << *base_type << "\n";
        // module->print(llvm::outs(), nullptr);
        valueType = base_type->getPointerElementType(); // pointer has no inbound check
        value = builder->CreateGEP(valueType, info1.value, info2.value);
    }
    if(node->inferred_type->kind != TypeKind::Array) {
        value = value = builder->CreateLoad(valueType, value);
    }
    return codeGenInfo{
        .value = value
    };
}


codeGenInfo codeGen::analyze(fun_call* node) {
    std::vector<llvm::Value*> args;
    for (auto& arg : node->args) {
        auto argVal = arg->dispatch(this).value;
        args.push_back(argVal);
    }
    if(!global->isInCurEnv(node->func_name)) {
        std::cout << "codeGen fun_call analyze fail\n";
        exit(1);
    }
    auto fn = llvm::dyn_cast<llvm::Function>(global->lookup(node->func_name).value);
    if(!fn) {
        std::cout << "codeGen fun_call analyze fail\n";
        exit(1);
    }
    auto call = builder->CreateCall(fn, args);
    return codeGenInfo{
        .value = call
    };
}

/*
Handling typecasting.
1. Array -> Pointer to the subarray
2. Int -> Float
*/
codeGenInfo codeGen::analyze(type_cast *node) {
    auto value = node->exp->dispatch(this).value;
    if(node->exp->inferred_type->kind == TypeKind::Array && node->target->kind == TypeKind::Pointer) { //pointer decay
        value = builder->CreateInBoundsGEP(to_llvm_type(node->exp->inferred_type), value, {builder->getInt32(0), builder->getInt32(0)});
        llvm::outs() << *value << *to_llvm_type(node->exp->inferred_type) << "\n";
        return codeGenInfo{
            .value = value
        };
    }
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
    auto cond = node->cond->dispatch(this).value;

    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*ctx, "then", currentFn);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*ctx, "else", currentFn);
    llvm::BasicBlock *MeetBB = nullptr;
    if(node->else_branch) {
        MeetBB = llvm::BasicBlock::Create(*ctx, "meet");
    }


    if(node->cond->inferred_type->kind == TypeKind::Int) {
        cond = builder->CreateICmpNE(cond, builder->getInt32(0));
        builder->CreateCondBr(cond, ThenBB, ElseBB);
    }

    builder->SetInsertPoint(ThenBB);
    node->if_branch->dispatch(this);
    if(node->else_branch) {
        terminateBlockWithBr(MeetBB, builder.get());
    } else {
        terminateBlockWithBr(ElseBB, builder.get());
    }

    if(node->else_branch) {
        builder->SetInsertPoint(ElseBB);
        node->else_branch->dispatch(this);
        terminateBlockWithBr(MeetBB, builder.get());
        currentFn->getBasicBlockList().push_back(MeetBB);
        builder->SetInsertPoint(MeetBB);
    } else {
        builder->SetInsertPoint(ElseBB);
    }

    return codeGenInfo();
}

/*

.loop_start

cond

.loop_body

body

.loop_end

remaining code
*/
codeGenInfo codeGen::analyze(while_stmt* node) {
    auto oldLoopStart = curLoopStart;
    auto oldLoopEnd = curLoopEnd;

    curLoopStart = llvm::BasicBlock::Create(*ctx, "loop_start", currentFn);
    terminateBlockWithBr(curLoopStart, builder.get());
    builder->SetInsertPoint(curLoopStart);
    auto cond = node->cond->dispatch(this).value;

    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(*ctx, "loop_body", currentFn);
    curLoopEnd = llvm::BasicBlock::Create(*ctx, "loop_end", currentFn);
    if(node->cond->inferred_type->kind == TypeKind::Int) {
        cond = builder->CreateICmpNE(cond, builder->getInt32(0));
        builder->CreateCondBr(cond, LoopBody, curLoopEnd);
    }
    builder->SetInsertPoint(LoopBody);
    node->body->dispatch(this);
    terminateBlockWithBr(curLoopStart, builder.get());
    builder->SetInsertPoint(curLoopEnd);

    curLoopStart = oldLoopStart;
    curLoopEnd = oldLoopEnd;

    return codeGenInfo();
}

codeGenInfo codeGen::analyze(break_stmt* node) {
    if(!curLoopEnd) {
        std::cout << "break_stmt analyze fail\n";
        exit(1);
    }
    builder->CreateBr(curLoopEnd);
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(continue_stmt* node) {
    if(!curLoopStart) {
        std::cout << "break_stmt analyze fail\n";
        exit(1);
    }
    builder->CreateBr(curLoopStart);
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
    auto cls = llvm::StructType::create(*ctx, node->name);
    std::vector<llvm::Type*> fields;
    for(auto pair : node->type->fields) {
        auto fieldName = pair.first;
        auto fieldInfo = pair.second;
        std::cout << fieldName << fieldInfo.type->to_string() << "\n";
        fields.push_back(to_llvm_type(fieldInfo.type));
    }
    cls->setBody(fields);

    new llvm::GlobalVariable(
    *module,
    cls,  // The class type
    false, // isConstant
    llvm::GlobalValue::ExternalLinkage,
    nullptr, // Initializer
    node->name + "_type_anchor");
    
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
        var_builder->CreateStore(&arg, alloca);
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

    currentFn->viewCFG(); 

    llvm::verifyFunction(*currentFn);
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(var_def* node) {
    auto type = to_llvm_type(node->type);
    var_builder->SetInsertPoint(&currentFn->getEntryBlock(), currentFn->getEntryBlock().begin());
    auto addr = var_builder->CreateAlloca(type, nullptr, node->id);
    if(node->init_val) {
        init_val *val = dynamic_cast<init_val*>(node->init_val.get());
        if(val->scalar) {
            auto scalar = val->scalar->dispatch(this).value;
            builder->CreateStore(scalar, addr);
        } else if(val->children.size() > 0) {
            if(node->type->kind != TypeKind::Array) {
                std::cout << "var_def array type error\n";
                exit(1);
            }
            ArrayType *arr = dynamic_cast<ArrayType*>(node->type);
            if(!arr->is_const) {
                llvm::Value *array_base = addr;
                for(size_t i = 0; i < arr->dims.size(); i++) {
                    // llvm::outs() << *array_base << "\n";
                    array_base = builder->CreateInBoundsGEP(array_base->getType()->getPointerElementType(), array_base, {builder->getInt32(0), builder->getInt32(0)});
                }
                for(size_t i = 0; i < val->children.size(); i++) {
                    auto llvm_elem_var = val->children[i]->scalar->dispatch(this).value;
                    builder->CreateStore(llvm_elem_var, array_base);
                    array_base = builder->CreateInBoundsGEP(array_base->getType()->getPointerElementType(), array_base, builder->getInt32(1)); 
                }
            }
        } else {
            std::cout << "var_def codeGen analyze fail\n";
            exit(1);
        }
    }
    // module->print(llvm::outs(), nullptr);
    cur->insert(node->id, env_info{
        addr
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
    module->print(llvm::outs(), nullptr);
    if (hasErrors) {
        llvm::errs() << "Module verification failed!\n";
        exit(1);
    }
    saveModuleToFile(outFileName);
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

void codeGen::terminateBlockWithBr(llvm::BasicBlock *target, llvm::IRBuilder<>* builder) {
    if(builder->GetInsertBlock()->getTerminator() == nullptr) {
        builder->CreateBr(target);
    }
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
            llvm::Type *array = elementType;// from clang
            for(auto it = arrayType->dims.rbegin(); it != arrayType->dims.rend(); it++) {
                array = llvm::ArrayType::get(array, *it);
            }
            return array;
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
