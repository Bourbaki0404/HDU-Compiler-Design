#ifndef __NODE_H
#define __NODE_H

#include "common/common.hpp"


struct node;
struct program;
struct expr;
struct stmt;
struct lval_expr;
struct block_stmt;
struct func_def;
struct init_val;
struct class_def;
struct pointer_acc;

struct constInfo;
struct TypeChecker;

enum ASTKind {
    Program,
    Int_Literal,
    Unary_Op,
    Binary_Op,
    Identifier,
    Node,
    Token,
    LVAL,
    Expr_Stmt,
    IF_ELSE_Stmt,
    WHILE_Stmt,
    Break_Stmt,
    Continue_Stmt,
    Return_Stmt,
    Assign_Stmt,
    Fun_Call,
    Func_Def,
    Var_Def,
    Var_Decl,
    Init_Val,
    Class_Def,
    Type_Cast
};



struct typeEvaluator {
    virtual void evaluateType () = 0;
};

struct constEvaluator {
    virtual constInfo const_eval(TypeChecker *ptr) = 0;
};

using progPtr = std::unique_ptr<program>;
using nodePtr = std::unique_ptr<node>;
using expPtr = std::unique_ptr<expr>;
using lvalPtr = std::unique_ptr<lval_expr>;
using stmtPtr = std::unique_ptr<stmt>;
using blockPtr = std::unique_ptr<block_stmt>;
using initValPtr = std::unique_ptr<init_val>;
using classPtr = std::unique_ptr<class_def>;

#include "types/types.hpp"
#include "types/TypeChecker.hpp"
#include "codeGen/codeGen.hpp"

std::string locToString(std::pair<size_t, size_t> location, std::string tail);

struct node {
    public:
        node(std::pair<size_t, size_t> loc);
        virtual std::string to_string() = 0;
        virtual void printAST(std::string prefix, std::string info_prefix) = 0;
        virtual analyzeInfo dispatch(TypeChecker *ptr) = 0;
        virtual codeGenInfo dispatch(codeGen *ptr) = 0;
    public:
        ASTKind kind = Node;
        std::pair<size_t, size_t> location;
        std::string error_msg;
};

struct program : public node{
    public:
        program(std::pair<size_t, size_t> loc);
        std::string to_string() override;
        void printAST(std::string prefix, std::string info_prefix) override;
        analyzeInfo dispatch(TypeChecker *ptr) ;
        codeGenInfo dispatch(codeGen *ptr);
    public:
        std::vector<node*> children;
};


struct expr : public node, public constEvaluator {
    public:
        expr(std::pair<size_t, size_t> loc, struct Type *ptr);
        void setConst(bool is_const);
    public:
        struct Type *inferred_type;
        bool is_const;
};

struct unary_expr : public expr {
    unary_expr(std::pair<size_t, size_t> loc, std::string op, expPtr operand);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr) ;
    std::string op;
    expPtr operand;
};

struct binary_expr : public expr {
    binary_expr(std::pair<size_t, size_t> loc, std::string op,
               expPtr left, expPtr right);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr) ;
    std::string op;
    expPtr left;
    expPtr right;
};

struct subscript_expr : public expr {
    subscript_expr(std::pair<size_t, size_t> loc, expPtr list, expPtr sub);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr) ;
    struct Type *base_type = nullptr;
    expPtr list = nullptr;
    expPtr sub = nullptr;
};

struct identifier : public expr {
    identifier(std::pair<size_t, size_t> loc, std::string name);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr) ;
    std::string name;
};

// struct lval_expr : public expr {
//     lval_expr();
//     std::string to_string() override;
//     void printAST(std::string prefix, std::string info_prefix) override;
//     void setIdAndReverseDim(std::string id);
//     void addDim(expPtr ptr);
//     void setLoc(std::pair<size_t, size_t> loc);
//     analyzeInfo dispatch(TypeChecker *ptr) override;
//     codeGenInfo dispatch(codeGen *ptr);
//     constInfo const_eval(TypeChecker *ptr) ;
//     std::string id;
//     std::vector<expPtr> dims;
//     struct Type *id_type = nullptr;
// };


struct stmt : public node {
    stmt(std::pair<size_t, size_t> loc);
};

struct expr_stmt : public stmt {
    expr_stmt(std::pair<size_t, size_t> loc, expPtr ptr);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    expPtr ptr;
};

struct if_else_stmt : public stmt {
    if_else_stmt(std::pair<size_t, size_t> loc, expPtr cond,
                stmtPtr if_branch, stmtPtr else_branch = nullptr);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    const ASTKind kind = IF_ELSE_Stmt;
    expPtr cond;
    stmtPtr if_branch;
    stmtPtr else_branch;
};

struct while_stmt : public stmt {
    while_stmt(std::pair<size_t, size_t> loc, expPtr cond,
              stmtPtr body);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    expPtr cond;
    stmtPtr body;
};


struct break_stmt : public stmt {
public:
    break_stmt(std::pair<size_t, size_t> loc) ;
    std::string to_string() override ;
    void printAST(std::string prefix, std::string info_prefix) override ;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
};

struct continue_stmt : public stmt {
public:
    continue_stmt(std::pair<size_t, size_t> loc);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
};

struct return_stmt : public stmt {
public:
    return_stmt(std::pair<size_t, size_t> loc, expPtr value = nullptr) ;
    std::string to_string() override ;
    void printAST(std::string prefix, std::string info_prefix) override ;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
public:
    expPtr value;
};

struct block_stmt : public stmt {
    block_stmt(std::pair<size_t, size_t> loc);
    void add_item(nodePtr item);
    void setLoc(std::pair<size_t, size_t> loc);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ; 
    codeGenInfo dispatch(codeGen *ptr);
    std::vector<nodePtr> items;
};



struct literal : public expr {
    public:
        literal(std::pair<size_t, size_t> loc);
        void printAST(std::string prefix, std::string info_prefix) override;
};

struct bool_literal : public literal {

};

struct int_literal : public literal {
    public:
        int_literal(std::pair<size_t, size_t> loc, int val);     
        std::string to_string() override ;
        analyzeInfo dispatch(TypeChecker *ptr) ;
        codeGenInfo dispatch(codeGen *ptr);
        constInfo const_eval(TypeChecker *ptr) ;
    public:
        const ASTKind kind = Int_Literal;
        int value;
};



// void printASTNode(nodePtr root);


struct fun_call : public expr {
public:
    fun_call(std::pair<size_t, size_t> loc);
    void addParam(expPtr param);
    void setName(std::string name);
    std::string to_string() override ;
    void setLoc(std::pair<size_t, size_t> loc) ;
    void printAST(std::string prefix, std::string info_prefix) override ;
    analyzeInfo dispatch(TypeChecker *ptr) override;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr) ;
public:
    const ASTKind kind = Fun_Call;
    std::string func_name;
    std::vector<expPtr> args;
};


struct func_def : public node {
    func_def(std::pair<size_t, size_t> loc);
    void set_body(blockPtr body);
    std::string to_string() override;
    void printArgList(std::string prefix, std::string info_prefix);
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    void setCtor();
    FuncType *type; //will be evaluated during type checking
    std::string name;
    std::vector<nodePtr> body;
    bool is_constructor = false;
};

struct var_def : public node {
    var_def(std::pair<size_t, size_t> loc);
    void setId(const std::string& name);
    void setConst(bool is_const);
    void setInitVal(nodePtr val);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    void finalizeType(std::string type_name);
    std::string id;
    struct Type *type;
    nodePtr init_val;
    bool is_const = false;
};

using vardefPtr = std::unique_ptr<var_def>;

struct var_decl : public node {
    var_decl(std::pair<size_t, size_t> loc);
    void addVarDef(vardefPtr def);
    void setTypeAndReverse(std::string name);
    void setLoc(std::pair<size_t, size_t> loc);
    void setConst(bool is_const);
    std::string to_string() override ;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    bool is_const = false;
    std::string typeName;
    std::vector<vardefPtr> defs;
private:
    void finalize();
};

struct init_val : node {
    init_val(std::pair<size_t, size_t> loc);
    void addChild(initValPtr child);
    void setLoc(size_t row, size_t col);
    void reverseChildren() ;
    void setConst(bool is_const);
    void setScalar(expPtr val);
    std::string to_string();
    void setLoc(std::pair<size_t, size_t>);
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
public:
    ASTKind kind = Init_Val;
    bool is_const = false;
    std::vector<initValPtr> children;
    expPtr scalar;
};

struct class_def : node {
public:
    class_def(std::pair<size_t, size_t> loc);
    void addChild(nodePtr child);
    void setName(std::string name);
    void setLoc(std::pair<size_t, size_t> loc);
    std::string to_string() override;
    void reverseChildren() ;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
public:
    std::string name;
    std::vector<nodePtr> children;
    //only will be filled in by typechecker
    class_def *type = nullptr;
};

struct member_access : expr {
    member_access(std::pair<size_t, size_t> loc, expPtr exp, const std::string &name);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr);
    std::string name;
    expPtr exp = nullptr;
    bool isFunc = false;
    std::vector<expPtr> args;
};

struct pointer_acc : expr {
    pointer_acc(std::pair<size_t, size_t> loc, expPtr exp, const std::string &name);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr);
    std::string name;
    expPtr exp = nullptr;
    bool isFunc = false;
    std::vector<expPtr> args;
};

struct type_cast : expr {
    type_cast(std::pair<size_t, size_t> loc, expPtr exp, struct Type *target);
    type_cast(expPtr exp, struct Type *target);
    std::string to_string() override;
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    constInfo const_eval(TypeChecker *ptr);
    struct Type* target;
    expPtr exp = nullptr;
    bool isImplicit = true;
};

#endif