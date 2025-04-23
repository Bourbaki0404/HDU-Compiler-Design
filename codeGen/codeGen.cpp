#include "parser/astnodes/node.hpp"

codeGen::codeGen() {
    moduleInit();
    setOutputFileName("out.ll");
}

void codeGen::setOutputFileName(const std::string &name) {
    outFileName = name;
} 

// Expression nodes
codeGenInfo codeGen::analyze(binary_expr* node) {
    // TODO: Implement binary expression code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(unary_expr* node) {
    // TODO: Implement unary expression code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(lval_expr* node) {
    // TODO: Implement lvalue expression code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(fun_call* node) {
    // TODO: Implement function call code generation
    return codeGenInfo();
}

// Literal nodes
codeGenInfo codeGen::analyze(int_literal* node) {
    // TODO: Implement integer literal code generation
    return codeGenInfo();
}

// Statement nodes
codeGenInfo codeGen::analyze(expr_stmt* node) {
    // TODO: Implement expression statement code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(if_else_stmt* node) {
    // TODO: Implement if-else statement code generation
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
    // TODO: Implement return statement code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(block_stmt* node) {
    // TODO: Implement block statement code generation
    return codeGenInfo();
}

// Declaration/definition nodes
codeGenInfo codeGen::analyze(class_def* node) {
    // TODO: Implement class definition code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(func_def* node) {
    // TODO: Implement function definition code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(var_def* node) {
    // TODO: Implement variable definition code generation
    return codeGenInfo();
}

codeGenInfo codeGen::analyze(var_decl* node) {
    // TODO: Implement variable declaration code generation
    return codeGenInfo();
}

// Program node
codeGenInfo codeGen::analyze(program* node) {
    auto bytePtrTy = builder->getInt8Ty()->getPointerTo();
    module->getOrInsertFunction("printf", llvm::FunctionType::get(builder->getInt32Ty(), bytePtrTy, true));

    auto fnType = llvm::FunctionType::get(builder->getInt32Ty(), false);
    currentFn = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, "main", *module);
    auto entry = llvm::BasicBlock::Create(*ctx, "entry", currentFn);
    builder->SetInsertPoint(entry);

    llvm::Value *str = builder->CreateGlobalStringPtr("hello, world!\n");
    auto printFn = module->getFunction("printf");
    std::vector<llvm::Value*> args = {str};
    builder->CreateCall(printFn, args);

    builder->CreateRet(builder->getInt32(32));

    // builder->CreateRet(result);
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
}