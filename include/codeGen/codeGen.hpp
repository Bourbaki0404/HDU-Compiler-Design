#ifndef __CODE_GEN_H
#define __CODE_GEN_H

#include "common/common.hpp"

#include "llvm-14/llvm/IR/Module.h"
#include "llvm-14/llvm/IR/LLVMContext.h"
#include "llvm-14/llvm/IR/IRBuilder.h"
#include "llvm-14/llvm/IR/Verifier.h"

struct node;
struct typeEvaluator;
struct program;
struct expr;
struct unary_expr;
struct binary_expr;
struct lval_expr;
struct stmt;
struct expr_stmt;
struct if_else_stmt;
struct while_stmt;
struct break_stmt;
struct continue_stmt;
struct return_stmt;
struct block_stmt;
struct literal;
struct bool_literal;
struct int_literal;
struct fun_call;
struct func_def;
struct var_def;
struct var_decl;
struct init_val;
struct class_def;
struct member_access;
struct pointer_acc;


struct Type;
struct PrimitiveType;
struct ArrayType;
struct FuncType;
struct ClassType;

struct SymbolTable;
struct Symbol;

struct environment;

struct codeGenInfo {
    llvm::Value *value;
};

struct codeGen {
    codeGen();

    void setOutputFileName(const std::string &name);

    codeGenInfo analyze(class_def* node) ;

    // Expression nodes
    codeGenInfo analyze(binary_expr* node) ;
    codeGenInfo analyze(unary_expr* node) ;
    codeGenInfo analyze(lval_expr* node) ;
    codeGenInfo analyze(fun_call* node) ;

    // Literal nodes
    codeGenInfo analyze(int_literal* node) ;

    // Statement nodes
    codeGenInfo analyze(expr_stmt* node) ;
    codeGenInfo analyze(if_else_stmt* node) ;
    codeGenInfo analyze(while_stmt* node) ;
    codeGenInfo analyze(break_stmt* node) ;
    codeGenInfo analyze(continue_stmt* node) ;
    codeGenInfo analyze(return_stmt* node) ;
    codeGenInfo analyze(block_stmt* node) ;

    // Declaration/definition nodes
    codeGenInfo analyze(func_def* node) ;
    codeGenInfo analyze(var_def* node) ;
    codeGenInfo analyze(var_decl* node) ;

    // Program node
    codeGenInfo analyze(program* node) ;

    // Initialization node
    codeGenInfo analyze(init_val* node) ;

    codeGenInfo analyze(member_access *node);
    codeGenInfo analyze(pointer_acc *node);

    // void analyzeFunctionBody(func_def *node);
    // void analyzeInit(var_def* node);
private:
    void moduleInit();
    void saveModuleToFile(const std::string &filename) {
        std::error_code error_code;
        llvm::raw_fd_ostream outLL(filename, error_code);
        module->print(outLL, nullptr);
    }
    codeGenInfo analyzeAdd(binary_expr *node);
    codeGenInfo analyzeMul(binary_expr *node);
    codeGenInfo analyzeCompare(binary_expr *node);
    codeGenInfo analyzeEq(binary_expr *node);
    codeGenInfo analyzeGt(binary_expr *node);

    llvm::Type *to_llvm_type(Type *ptr);

    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::IRBuilder<>> var_builder;
    std::unique_ptr<llvm::Module> module;

    llvm::Function *currentFn;
    std::string outFileName;

    environment *cur;
    environment *global;
};

struct env_info {
    llvm::Value *value;
};

struct environment {
    environment(struct environment *parent);
    environment();
    struct environment *parent;
    std::map<std::string, env_info> mapping;

    void insert(std::string name, env_info info);
    env_info lookup(std::string name);
    bool isExist(std::string name);
    bool isInCurEnv(std::string name);
};

using envPtr = std::shared_ptr<environment>;

#endif