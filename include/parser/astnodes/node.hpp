#ifndef __NODE_H
#define __NODE_H

#include "common/common.hpp"
#include "types/types.hpp"

struct node;
struct program;
struct expr;
struct stmt;
struct lval_expr;

enum ASTKind {
    Program,
    Int_Literal,
    Unary_Op,
    Binary_Op,
    Identifier,
    Node,
    Token,
    LVAL,
    Expr_Stmt
};

struct node {
    public:
        node(size_t row, size_t col)
        : location({row, col}) {}
        virtual std::string to_string() {}
        virtual void printAST(std::string prefix, std::string info_prefix) {}
    public:
        const ASTKind kind = Node;
        std::pair<size_t, size_t> location;
};


using nodePtr = std::unique_ptr<node>;
using expPtr = std::unique_ptr<expr>;
using lvalPtr = std::unique_ptr<lval_expr>;
using stmtPtr = std::unique_ptr<stmt>;

std::string locToString(std::pair<size_t, size_t> location);

struct program : public node{
    public:
        program(size_t row, size_t col)
        : node(row, col) {}
        
        std::string to_string() override {
            return "Program";
        }

        void printAST(std::string prefix, std::string info_prefix) override{
            std::cout << prefix << to_string() << locToString(location);
            for(size_t i = 0; i < children.size(); i++) {
                if(i != children.size() - 1) {
                    children[i]->printAST(prefix + "├── ","");
                } else {
                    children[i]->printAST(prefix + "└── ","");
                }
            }
        }

    public:
        const ASTKind kind = Program;
        std::vector<node*> children;
};


struct expr : public node {
    public:
        expr(size_t row, size_t col) 
        : node(row, col) {}
    public:
        // type type;
};

struct unary_expr : public expr {
    public:
        unary_expr(size_t row, size_t col, 
                    std::string op, expPtr operand)
        : expr(row, col), operand(std::move(operand)) {}
        std::string to_string() override {
            return "unary_expr <op " + op + ">";
        }
        void printAST(std::string prefix, std::string info_prefix) override {
            std::cout << prefix << to_string() << locToString(location);
            operand->printAST(prefix + "└── ", "");
        }
    public:
        const ASTKind kind = Unary_Op;
        std::string op;
        expPtr operand;
};

struct binary_expr : public expr {
    public:
        binary_expr(size_t row, size_t col, 
                    std::string op, expPtr left, expPtr right)
        : expr(row, col), op(op), left(std::move(left)), right(std::move(right)) {}
        virtual std::string to_string() {
            return "binary_expr <op " + op + ">";
        }
        virtual void printAST(std::string prefix, std::string info_prefix) {
            std::cout << info_prefix << to_string() << locToString(location);
            left->printAST(prefix + "│   ", prefix + "├── ");
            right->printAST(prefix + "    ", prefix + "└── ");
        }
    public:
        const ASTKind kind = Binary_Op;
        std::string op;
        expPtr left;
        expPtr right;
};

struct lval_expr : public expr {
    public:
        lval_expr()
        : expr(-1,-1) {}

        std::string to_string() override {
            return "LVal <id " + id + ">";
        }

        void printAST(std::string prefix, std::string info_prefix) override {
            std::cout << info_prefix << to_string() << locToString(location);
            for (size_t i = 0; i < dims.size(); ++i) {
                if(i != dims.size() - 1) {
                    dims[i]->printAST(prefix + "│   ", prefix + "├── ");
                } else {
                    dims[i]->printAST(prefix + "    ", prefix + "└── ");
                }
            }
        }
        
        void setIdAndReverseDim(std::string id) {
            this->id = id;
            std::reverse(dims.begin(), dims.end());
        }

        void addDim(expPtr ptr) {
            this->dims.push_back(std::move(ptr));
        }

        void setLoc(size_t row, size_t col) {
            this->location.first = row;
            this->location.second = col;
        }
    

    public:
        const ASTKind kind = LVAL;
        std::string id;
        std::vector<expPtr> dims;
};

struct stmt : public node {
    stmt(size_t row, size_t col)
    : node(row, col) {}
};

struct expr_stmt : public stmt {
    public:
        expr_stmt(size_t row, size_t col, expPtr ptr)
        : stmt(row, col), ptr(std::move(ptr)){}
        std::string to_string() override{
            return "expr_stmt";
        }
        void printAST(std::string prefix, std::string info_prefix) override {
            std::cout << info_prefix << to_string() << locToString(location);
            ptr->printAST(prefix + "    ", prefix + "└── ");
        }
    public:
        expPtr ptr;
};

struct if_else_stmt : public stmt {
public:
    if_else_stmt(size_t row, size_t col, expPtr cond, stmtPtr if_branch, stmtPtr else_branch = nullptr)
        : stmt(row, col), cond(std::move(cond)), if_branch(std::move(if_branch)), else_branch(std::move(else_branch)) {}

    std::string to_string() override {
        return else_branch ? "if_else_stmt" : "if_stmt";
    }

    void printAST(std::string prefix, std::string info_prefix) override {
        std::cout << info_prefix << to_string() << locToString(location);
        cond->printAST(prefix + "    ", prefix + "├──(cond) ");
        if_branch->printAST(prefix + (else_branch ? "│   " : "    "), 
                            prefix + (else_branch ? "├──(if) " : "└──(if) "));
        if (else_branch) {
            else_branch->printAST(prefix + "    ", prefix + "└──(else) ");
        }
    }

public:
    expPtr cond;
    stmtPtr if_branch;
    stmtPtr else_branch; // nullable for simple if
};

struct while_stmt : public stmt {
public:
    while_stmt(size_t row, size_t col, expPtr cond, stmtPtr body)
        : stmt(row, col), cond(std::move(cond)), body(std::move(body)) {}

    std::string to_string() override {
        return "while_stmt";
    }

    void printAST(std::string prefix, std::string info_prefix) override {
        std::cout << info_prefix << to_string() << locToString(location);
        cond->printAST(prefix + "    ", prefix + "├──(cond) ");
        body->printAST(prefix + "    ", prefix + "└──(body) ");
    }

public:
    expPtr cond;
    stmtPtr body;
};

struct break_stmt : public stmt {
public:
    break_stmt(size_t row, size_t col) : stmt(row, col) {}

    std::string to_string() override {
        return "break_stmt";
    }

    void printAST(std::string prefix, std::string info_prefix) override {
        std::cout << info_prefix << to_string() << locToString(location);
    }
};

struct continue_stmt : public stmt {
public:
    continue_stmt(size_t row, size_t col) : stmt(row, col) {}

    std::string to_string() override {
        return "continue_stmt";
    }

    void printAST(std::string prefix, std::string info_prefix) override {
        std::cout << info_prefix << to_string() << locToString(location);
    }
};

struct return_stmt : public stmt {
public:
    return_stmt(size_t row, size_t col, expPtr value = nullptr)
        : stmt(row, col), value(std::move(value)) {}

    std::string to_string() override {
        return "return_stmt";
    }

    void printAST(std::string prefix, std::string info_prefix) override {
        std::cout << info_prefix << to_string() << locToString(location);
        if (value) {
            value->printAST(prefix + "    ", prefix + "└──(value) ");
        }
    }

public:
    expPtr value;
};

struct block_stmt : public stmt {
public:
    block_stmt(size_t row, size_t col)
        : stmt(row, col) {}

    void add_item(nodePtr item) {
        if (item) items.push_back(std::move(item));
    }

    std::string to_string() override {
        return "block_stmt";
    }

    void printAST(std::string prefix, std::string info_prefix) override {
        std::cout << info_prefix << to_string() << locToString(location);
        for (size_t i = 0; i < items.size(); ++i) {
            if (i != items.size() - 1) {
                items[i]->printAST(prefix + "│   ", prefix + "├── ");
            } else {
                items[i]->printAST(prefix + "    ", prefix + "└── ");
            }
        }
    }
public:
    std::vector<nodePtr> items;
};



struct identifier_expr : public expr {
public:
    identifier_expr(size_t row, size_t col, std::string name)
    : expr(row, col), name(std::move(name)) {}

    std::string to_string() override {
        return "identifier_expr <name " + name + ">";
    }
    void printAST(std::string prefix, std::string info_prefix) override {
        std::cout << prefix << "── " << to_string() << locToString(location);
    }
public:
    const ASTKind kind = Identifier;
    std::string name;
};

struct literal : public expr {
    public:
        literal(size_t row, size_t col)
        : expr(row, col) {}
        void printAST(std::string prefix, std::string info_prefix) override {
            std::cout << info_prefix << this->to_string() << locToString(location);
        }
};

struct bool_literal : public literal {

};

struct int_literal : public literal {
    public:
        int_literal(size_t row, size_t col, int val)
        : literal(row, col), value(val) {}     
        std::string to_string() override{
            return "int_literal <value " + std::to_string(value) + ">"; 
        }
        // void printAST(std::string prefix) {
        //     std::cout << prefix << to_string() << locToString(location);
        // }   
    public:
        const ASTKind kind = Int_Literal;
        int value;
};



// void printASTNode(nodePtr root);




#endif