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
struct func_param;
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
// template<typename T>
// struct  AbstractNodeAnalyzer {
//     // T analyze( node);
//     // T analyze(BooleanLiteral node);
//     // T analyze(ClassDef node);
//     // T analyze(ClassType node);
//     // T analyze(CompilerError node);
//     // T analyze(Errors node);
//     // T analyze( node);
//     // T analyze(ListType node);
//     // T analyze(MemberExpr node);
//     // T analyze(MethodCallExpr node);
//     // T analyze(NonLocalDecl node);
//     // T analyze(TypedVar node);

//     // Expression nodes
//     virtual T analyze(binary_expr* node) = 0;
//     virtual T analyze(unary_expr* node) = 0;
//     virtual T analyze(lval_expr* node) = 0;
//     virtual T analyze(identifier_expr* node) = 0;
//     virtual T analyze(fun_call* node) = 0;

//     // Literal nodes
//     virtual T analyze(int_literal* node) = 0;
//     // T analyze(float_literal* node);
//     // T analyze(char_literal* node);
//     // T analyze(string_literal* node);
//     // T analyze(bool_literal* node);

//         // Statement nodes
//     virtual T analyze(expr_stmt* node) = 0;
//     virtual T analyze(if_else_stmt* node) = 0;
//     virtual T analyze(while_stmt* node) = 0;
//     virtual T analyze(break_stmt* node) = 0;
//     virtual T analyze(continue_stmt* node) = 0;
//     virtual T analyze(return_stmt* node) = 0;
//     virtual T analyze(block_stmt* node) = 0;

//     // Declaration/definition nodes
//     virtual T analyze(func_def* node) = 0;
//     virtual T analyze(var_def* node) = 0;
//     virtual T analyze(var_decl* node) = 0;
//     virtual T analyze(func_param* node) = 0;
    
//     // Program node
//     virtual T analyze(program* node) = 0;
    
//     // Initialization node
//     virtual T analyze(init_val* node) = 0;

//     /** Set the default value returned by calls to analyze that are not
//      *  overridden to VALUE. By default, this is null. */
//     virtual void setDefault(T value) = 0;
// };

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
    analyzeInfo analyze(func_param* node) ;

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
private:
    void TypeError(node *ptr, const std::string &str);
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