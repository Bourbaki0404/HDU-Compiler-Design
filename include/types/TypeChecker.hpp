#ifndef __TYPECHECKER_H
#define __TYPECHECKER_H

#include "common/common.hpp"
// #include "symbolTable/symbolTable.hpp"

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

struct analyzeInfo {
    struct Type *type = nullptr;
};

struct constInfo {
    bool is_const = false;
    void *value = nullptr;
    struct Type *type = nullptr;
};

struct TypeChecker {
    TypeChecker();
    analyzeInfo defaultResult;

    // Default value handling
    void setDefault(analyzeInfo value) {
        defaultResult = value;
    }

    analyzeInfo analyze(class_def* node) ;

    // Expression nodes
    analyzeInfo analyze(binary_expr* node) ;
    analyzeInfo analyze(unary_expr* node) ;
    analyzeInfo analyze(lval_expr* node) ;
    analyzeInfo analyze(fun_call* node) ;

    // Literal nodes
    analyzeInfo analyze(int_literal* node) ;

    // Statement nodes
    analyzeInfo analyze(expr_stmt* node) ;
    analyzeInfo analyze(if_else_stmt* node) ;
    analyzeInfo analyze(while_stmt* node) ;
    analyzeInfo analyze(break_stmt* node) ;
    analyzeInfo analyze(continue_stmt* node) ;
    analyzeInfo analyze(return_stmt* node) ;
    analyzeInfo analyze(block_stmt* node) ;

    // Declaration/definition nodes
    analyzeInfo analyze(func_def* node) ;
    analyzeInfo analyze(var_def* node) ;
    analyzeInfo analyze(var_decl* node) ;

    // Program node
    analyzeInfo analyze(program* node) ;

    // Initialization node
    analyzeInfo analyze(init_val* node) ;

    analyzeInfo analyze(member_access *node);
    analyzeInfo analyze(pointer_acc *node);

    void analyzeFunctionBody(func_def *node);
    void analyzeInit(var_def* node);

    // Visit types
    analyzeInfo evaluate(ArrayType* node) ;
    analyzeInfo evaluate(FuncType* node) ;

    // Evaluate Const
    constInfo const_eval(unary_expr *ptr);
    constInfo const_eval(binary_expr* node) ;
    constInfo const_eval(lval_expr* node) ;
    constInfo const_eval(fun_call* node) ;//always return false
    constInfo const_eval(int_literal* node);

    void dumpErrors(std::string path);
    bool hasTypeError();
private:
    void TypeError(node *ptr, const std::string &str);
    void analyzeAdd(binary_expr *ptr);
    void analyzeMul(binary_expr *ptr);
    void analyzeBitAnd(binary_expr *node);
    void analyzeLogicAnd(binary_expr *node);
    void analyzeCompare(binary_expr *node);
    void analyzeEq(binary_expr *node);
    // Type checking state
    func_def *currentFuncDef;
    class_def *currentClassDef;
    SymbolTable *symbolTable;
    analyzeInfo currentReturnType;
    std::vector<size_t> controlFlowStack;
    size_t loopDepth;
    std::vector<std::string> errorMessages;
};


#endif