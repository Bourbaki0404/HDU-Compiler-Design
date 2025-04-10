#include "parser/parser.hpp"

symbolType tokenToSymbol(token::tokenType tokType) {
    switch(tokType) {
        // Identifiers and literals
        case token::ID: return ID;
        case token::NUM: return NUM;
        case token::STRING: return STRING;

        // Brackets and delimiters
        case token::LPR: return LPR;
        case token::RPR: return RPR;
        case token::LBK: return LBK;
        case token::RBK: return RBK;
        case token::LBC: return LBC;
        case token::RBC: return RBC;
        case token::COMMA: return COMMA;
        case token::SEMICOLON: return SEMICOLON;
        case token::COLON: return COLON;
        case token::DOT: return DOT;

        // Operators
        case token::ASSIGN: return ASSIGN;
        case token::PLUS: return PLUS;
        case token::MINUS: return MINUS;
        case token::STAR: return STAR;
        case token::SLASH: return SLASH;
        case token::PERCENT: return PERCENT;
        case token::PLUS_PLUS: return PLUS_PLUS;
        case token::MINUS_MINUS: return MINUS_MINUS;
        case token::PLUS_EQ: return PLUS_EQ;
        case token::MINUS_EQ: return MINUS_EQ;
        case token::STAR_EQ: return STAR_EQ;
        case token::SLASH_EQ: return SLASH_EQ;
        case token::PERCENT_EQ: return PERCENT_EQ;
        case token::EQ: return EQ;
        case token::NEQ: return NEQ;
        case token::LT: return LT;
        case token::GT: return GT;
        case token::LTE: return LTE;
        case token::GTE: return GTE;
        case token::AND: return AND;
        case token::OR: return OR;
        case token::NOT: return NOT;
        case token::BIT_AND: return BIT_AND;
        case token::BIT_OR: return BIT_OR;
        case token::BIT_XOR: return BIT_XOR;
        case token::SHL: return SHL;
        case token::SHR: return SHR;

        // Keywords
        case token::KW_IF: return KW_IF;
        case token::KW_ELSE: return KW_ELSE;
        case token::KW_WHILE: return KW_WHILE;
        case token::KW_FOR: return KW_FOR;
        case token::KW_RETURN: return KW_RETURN;
        case token::KW_FUNCTION: return KW_FUNCTION;
        case token::KW_CLASS: return KW_CLASS;

        // Type keywords
        case token::KW_INT: return KW_INT;
        case token::KW_VOID: return KW_VOID;
        case token::KW_FLOAT: return KW_FLOAT;
        case token::KW_STRUCT: return KW_STRUCT;
        case token::KW_CHAR: return KW_CHAR;

        // Special
        case token::END_OF_FILE: return END_OF_FILE;
        case token::INVALID: return INVALID;

        default:
            throw std::runtime_error("Unknown token type");
    }
}

// std::vector<symbol> tokensToSymbols(std::vector<token> tokens) {
//     std::vector<symbol> res;
//     for (const auto &tok : tokens) {
//         res.push_back(symbol(tokenToSymbol(tok.type), tok.value, tok.location.first, tok.location.second));
//     }    
//     return res;
// }

template <typename Base, typename Derived>
std::unique_ptr<Base> convert_unique_ptr(std::unique_ptr<Derived> ptr) {
    return std::unique_ptr<Base>(static_cast<Base*>(ptr.release())); // Release and convert
}

const std::unordered_map<symbolType, std::string> operatorSymbols = {
    {symbolType::PLUS, "+"},
    {symbolType::MINUS, "-"},
    {symbolType::STAR, "*"},
    {symbolType::SLASH, "/"},
    {symbolType::PERCENT, "%"},
    {symbolType::EQ, "=="},
    {symbolType::NEQ, "!="},
    {symbolType::LT, "<"},
    {symbolType::GT, ">"},
    {symbolType::LTE, "<="},
    {symbolType::GTE, ">="},
    {symbolType::AND, "&&"},
    {symbolType::OR, "||"},
    {symbolType::NOT, "!"},
    {symbolType::BIT_AND, "&"},
    {symbolType::BIT_OR, "|"},
    {symbolType::BIT_XOR, "^"},
    {symbolType::SHL, "<<"},
    {symbolType::SHR, ">>"},
    {symbolType::ASSIGN, "="},
    {symbolType::PLUS_EQ, "+="},
    {symbolType::MINUS_EQ, "-="},
    {symbolType::STAR_EQ, "*="},
    {symbolType::SLASH_EQ, "/="},
    {symbolType::PERCENT_EQ, "%="},
    {symbolType::PLUS_PLUS, "++"},
    {symbolType::MINUS_MINUS, "--"},
    // ... (extend as needed)
};

const std::unordered_map<symbolType, std::string> symbolTypeNames = {
    {START, "START"},

    // Identifiers and literals
    {ID, "ID"},
    {NUM, "NUM"},
    {STRING, "STRING"},

    // Brackets and delimiters
    {LPR, "LPR"},
    {RPR, "RPR"},
    {LBK, "LBK"},
    {RBK, "RBK"},
    {LBC, "LBC"},
    {RBC, "RBC"},
    {COMMA, "COMMA"},
    {SEMICOLON, "SEMICOLON"},
    {COLON, "COLON"},
    {DOT, "DOT"},

    // Operators
    {ASSIGN, "ASSIGN"},
    {PLUS, "PLUS"},
    {MINUS, "MINUS"},
    {STAR, "STAR"},
    {SLASH, "SLASH"},
    {PERCENT, "PERCENT"},
    {PLUS_PLUS, "PLUS_PLUS"},
    {MINUS_MINUS, "MINUS_MINUS"},
    {PLUS_EQ, "PLUS_EQ"},
    {MINUS_EQ, "MINUS_EQ"},
    {STAR_EQ, "STAR_EQ"},
    {SLASH_EQ, "SLASH_EQ"},
    {PERCENT_EQ, "PERCENT_EQ"},
    {EQ, "EQ"},
    {NEQ, "NEQ"},
    {LT, "LT"},
    {GT, "GT"},
    {LTE, "LTE"},
    {GTE, "GTE"},
    {AND, "AND"},
    {OR, "OR"},
    {NOT, "NOT"},
    {BIT_AND, "BIT_AND"},
    {BIT_OR, "BIT_OR"},
    {BIT_XOR, "BIT_XOR"},
    {SHL, "SHL"},
    {SHR, "SHR"},

    // Keywords
    {KW_IF, "KW_IF"},
    {KW_ELSE, "KW_ELSE"},
    {KW_WHILE, "KW_WHILE"},
    {KW_FOR, "KW_FOR"},
    {KW_RETURN, "KW_RETURN"},
    {KW_FUNCTION, "KW_FUNCTION"},
    {KW_CLASS, "KW_CLASS"},

    // Type keywords
    {KW_INT, "KW_INT"},
    {KW_VOID, "KW_VOID"},
    {KW_FLOAT, "KW_FLOAT"},
    {KW_STRUCT, "KW_STRUCT"},
    {KW_CHAR, "KW_CHAR"},
    {KW_BREAK, "BREAK"},
    {KW_CONTINUE, "CONTINUE"},
    {KW_CONST, "CONST"},

    // Special
    {END_OF_FILE, "EOF"},
    {INVALID, "INVALID"},
    {EMPTY, "EMPTY"},

    // Nonterminals
    {CompUnit, "CompUnit"},
    {Decl, "Decl"},
    {ConstDecl, "ConstDecl"},
    {ConstDeclTail, "ConstDeclTail"},
    {Type, "Type"},
    {ConstDef, "ConstDef"},
    {ConstDefTail, "ConstDefTail"},
    {ConstInitVal, "ConstInitVal"},
    {ConstInitValTail, "ConstInitValTail"},
    {FuncFParamTailTail, "FuncFParamTailTail"},
    {ConstInitValTailTail, "ConstInitValTailTail"},
    {VarDecl, "VarDecl"},
    {VarDeclTail, "VarDeclTail"},
    {VarDef, "VarDef"},
    {VarDefGroup, "VarDefGroup"},
    {InitVal, "InitVal"},
    {InitValTail, "InitValTail"},
    {InitValTailTail, "InitValTailTail"},
    {FuncDef, "FuncDef"},
    {FuncFParams, "FuncFParams"},
    {FuncFParamsTail, "FuncFParamsTail"},
    {FuncFParam, "FuncFParam"},
    {FuncFParamTail, "FuncFParamTail"},
    {Block, "Block"},
    {BlockItem, "BlockItem"},
    {BlockItemTail, "BlockItemTail"},
    {Stmt, "Stmt"},
    {ReturnExp, "ReturnExp"},
    {Exp, "Exp"},
    {LVal, "LVal"},
    {LValTail, "LValTail"},
    {PrimaryExp, "PrimaryExp"},
    {Number, "Number"},
    {UnaryExp, "UnaryExp"},
    {UnaryOp, "UnaryOp"},
    {FuncRParams, "FuncRParams"},
    {FuncRParamsTail, "FuncRParamsTail"},
    {MulExp, "MulExp"},
    {MulOp, "MulOp"},
    {AddExp, "AddExp"},
    {AddOp, "AddOp"},
    {RelExp, "RelExp"},
    {RelOp, "RelOp"},
    {EqExp, "EqExp"},
    {EqOp, "EqOp"},
    {LAndExp, "LAndExp"},
    {LOrExp, "LOrExp"},
    {ConstExp, "ConstExp"}
};

SemanticAction binaryFactory() {
    return [](std::vector<parseInfoPtr>& children) {
        
        auto left = expPtr(static_cast<expr*>(children[0]->ptr.release()));
        auto right = expPtr(static_cast<expr*>(children[2]->ptr.release()));
        auto row = left->location.first;
        auto col = left->location.second;
        
        auto ptr = new binary_expr(row, col, operatorSymbols.at(children[1]->type), 
                                    std::move(left), std::move(right));
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        res->set_node(nodePtr(ptr));
        return res;
    };
}

SemanticAction unaryFactory() {
    return [](std::vector<parseInfoPtr>& children) {
        auto operand = expPtr(static_cast<expr*>(children[1]->ptr.release()));
        auto row = children[0]->location.first;
        auto col = children[1]->location.second;
        auto ptr = new unary_expr(row, col, symbolTypeNames.at(children[0]->type), 
                                    std::move(operand));
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        res->set_node(nodePtr(ptr));
        return res;
    };
}

SemanticAction funcDefFactory() {
    return [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);

        // Create a new function definition with the specified return type and name
        auto funcDef = static_cast<func_def*>(children[3]->ptr.release());
        funcDef->set_ret_type(children[0]->str_val);
        funcDef->set_id_reverse_params(children[1]->str_val);

        funcDef->setLoc(row, col);
        // Set the function body (block)
        funcDef->set_body(blockPtr(static_cast<block_stmt*>(children[5]->ptr.release())));
        res->set_node(funcDefPtr(funcDef));
        return res;
    };
}
const std::vector<ruleAction> ruleWithAction = {
    // Start Symbol
    rule(START, {CompUnit}),

    // Program Structure
    rule(CompUnit, {Decl}),
    rule(CompUnit, {FuncDef}),
    rule(CompUnit, {CompUnit, Decl}),
    rule(CompUnit, {CompUnit, FuncDef}),

    // Declarations
    rule(Decl, {ConstDecl}),
    rule(Decl, {VarDecl}),

    // Types
    rule(Type, {KW_INT}),

    // Constant Declarations
    rule(ConstDecl, {KW_CONST, Type, ConstDef, ConstDeclTail, SEMICOLON}),
    rule(ConstDeclTail, {COMMA, ConstDef, ConstDeclTail}),
    rule(ConstDeclTail, {}),

    // Variable Declarations
    rule(VarDecl, {Type, VarDef, VarDeclTail, SEMICOLON}),
    rule(VarDeclTail, {COMMA, VarDef, VarDeclTail}),
    rule(VarDeclTail, {}),

    // Definitions
    rule(ConstDef, {ID, ConstDefTail, ASSIGN, ConstInitVal}),
    rule(ConstDefTail, {LBK, ConstExp, RBK, ConstDefTail}),
    rule(ConstDefTail, {}),

    rule(VarDef, {ID, VarDefGroup}),
    rule(VarDef, {ID, VarDefGroup, ASSIGN, InitVal}),
    rule(VarDefGroup, {LBK, ConstExp, RBK, VarDefGroup}),
    ruleAction(rule(VarDefGroup, {}), [](std::vector<parseInfoPtr>&) {
        auto res = std::make_unique<parseInfo>(-1, -1);
        auto funcDef = std::make_unique<var_def>(-1, -1);
        res->set_node(std::move(funcDef));
        return res;
    }),

    // Initialization
    rule(ConstInitVal, {ConstExp}),
    rule(ConstInitVal, {LBC, ConstInitValTail, RBC}),
    rule(ConstInitValTail, {ConstInitVal, ConstInitValTailTail}),
    rule(ConstInitValTail, {}),
    rule(ConstInitValTailTail, {COMMA, ConstInitVal, ConstInitValTailTail}),
    rule(ConstInitValTailTail, {}),

    rule(InitVal, {Exp}),
    rule(InitVal, {LBC, InitValTail, RBC}),
    rule(InitValTail, {InitVal, InitValTailTail}),
    rule(InitValTail, {}),
    rule(InitValTailTail, {COMMA, InitVal, InitValTailTail}),
    rule(InitValTailTail, {}),

    // Function Definitions
    ruleAction(rule(FuncDef, {Type, ID, LPR, FuncFParams, RPR, Block}), funcDefFactory()),
    ruleAction(rule(FuncDef, {KW_VOID, ID, LPR, FuncFParams, RPR, Block}),funcDefFactory()),

    // Function Parameters
    ruleAction(rule(FuncFParams, {FuncFParam, FuncFParamsTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        auto tail = static_cast<func_def*>(children[1]->ptr.release());
        tail->add_param(funcparamPtr(static_cast<func_param*>(children[0]->ptr.release())));
        res->set_node(nodePtr(tail));
        return res;
    }),

    ruleAction(rule(FuncFParams, {}), [](std::vector<parseInfoPtr>&) {
        // Create a func_def with no parameters
        auto res = std::make_unique<parseInfo>(-1, -1);
        auto funcDef = std::make_unique<func_def>(-1, -1);

        // No parameters, just set the node
        res->set_node(std::move(funcDef));

        return res;
    }),
    ruleAction(rule(FuncFParamsTail, {COMMA, FuncFParam, FuncFParamsTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        auto tail = static_cast<func_def*>(children[2]->ptr.release());
        tail->add_param(funcparamPtr(static_cast<func_param*>(children[1]->ptr.release())));
        res->set_node(nodePtr(tail));
        return res;
    }),
    ruleAction(rule(FuncFParamsTail, {}),[](std::vector<parseInfoPtr>&) {
        // Create a func_def with no parameters
        auto res = std::make_unique<parseInfo>(-1, -1);
        auto funcDef = std::make_unique<func_def>(-1, -1);

        // No parameters, just set the node
        res->set_node(std::move(funcDef));

        return res;
    }),
    ruleAction(rule(FuncFParam, {Type, ID, FuncFParamTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        std::string type = children[0]->str_val;
        std::string id = children[1]->str_val;

        if (children[2]) {
            func_param* arr = static_cast<func_param*>(children[2]->ptr.release());
            arr->setType(type);
            arr->setId(id);
            res->set_node(funcparamPtr(arr));
        } else {
            func_param* base = new func_param(row, col);
            base->setId(id);
            base->setType(type);
            res->set_node(funcparamPtr(base));
        }

        return res;
    }),
    ruleAction(rule(FuncFParamTail, {LBK, RBK, FuncFParamTailTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        auto tail = static_cast<func_param*>(children[2]->ptr.release());
        tail->addDim(0);
        res->set_node(funcparamPtr(tail));
        return res;
    }),
    rule(FuncFParamTail, {}),
    ruleAction(rule(FuncFParamTailTail, {LBK, ConstExp, RBK, FuncFParamTailTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        auto tail = static_cast<func_param*>(children[3]->ptr.release());
        tail->addDim(0);
        res->set_node(nodePtr(tail));
        return res;
    }),
    ruleAction(rule(FuncFParamTailTail, {}), [](std::vector<parseInfoPtr>&) {
        parseInfoPtr res = std::make_unique<parseInfo>(-1, -1); //loc not used
        func_param *p = new func_param(-1, -1);
        res->set_node(nodePtr(p));
        return res;
    }),

    // Blocks and Statements
    ruleAction(rule(Block, {LBC, BlockItemTail, RBC}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;

        parseInfoPtr res = std::make_unique<parseInfo>(row, col);

        if (children[1] && children[1]->ptr) {
            block_stmt* blk = static_cast<block_stmt*>(children[1]->ptr.get());
            blk->setLoc(row, col);
            std::reverse(blk->items.begin(), blk->items.end());  // Reverse here!
            res->set_node(std::move(children[1]->ptr));
        } else {
            res->set_node(std::make_unique<block_stmt>(row, col));
        }

        return res;
    }),

    ruleAction(rule(BlockItemTail, {BlockItem, BlockItemTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;

        auto* blk = static_cast<block_stmt*>(children[1]->ptr.get());
        if (children[0]->ptr) {
            blk->add_item(std::move(children[0]->ptr));
        }

        auto res = std::make_unique<parseInfo>(row, col);
        res->set_node(std::move(children[1]->ptr));  // Return same block_stmt upwards
        return res;
    }),

    ruleAction(rule(BlockItemTail, {}), [](std::vector<parseInfoPtr>&) {
        auto res = std::make_unique<parseInfo>(0, 0);
        auto blk = std::make_unique<block_stmt>(0, 0);
        res->set_node(std::move(blk));
        return res;
    }),

    rule(BlockItem, {Decl}),
    rule(BlockItem, {Stmt}),

    // Statements
    rule(Stmt, {LVal, ASSIGN, Exp, SEMICOLON}),
    ruleAction(rule(Stmt, {Exp, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        expr *ptr = static_cast<expr*>(children[0]->ptr.release());
        expr_stmt* p = new expr_stmt(row, col, expPtr(ptr));
        res->set_node(nodePtr(p));
        return res;
    }),
    rule(Stmt, {SEMICOLON}),
    rule(Stmt, {Block}),
    
    ruleAction(rule(Stmt, {KW_IF, LPR, Exp, RPR, Stmt}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);

        expr* cond_ptr = static_cast<expr*>(children[2]->ptr.release());
        stmt* if_stmt_ptr = static_cast<stmt*>(children[4]->ptr.release());

        if_else_stmt* node = new if_else_stmt(row, col, expPtr(cond_ptr), stmtPtr(if_stmt_ptr));
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_IF, LPR, Exp, RPR, Stmt, KW_ELSE, Stmt}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);

        expr* cond_ptr = static_cast<expr*>(children[2]->ptr.release());
        stmt* if_stmt_ptr = static_cast<stmt*>(children[4]->ptr.release());
        stmt* else_stmt_ptr = static_cast<stmt*>(children[6]->ptr.release());

        if_else_stmt* node = new if_else_stmt(row, col, expPtr(cond_ptr), stmtPtr(if_stmt_ptr), stmtPtr(else_stmt_ptr));
        res->set_node(nodePtr(node));
        return res;
    }),

    ruleAction(rule(Stmt, {KW_WHILE, LPR, Exp, RPR, Stmt}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);

        expr* cond_ptr = static_cast<expr*>(children[2]->ptr.release());
        stmt* body_ptr = static_cast<stmt*>(children[4]->ptr.release());

        while_stmt* node = new while_stmt(row, col, expPtr(cond_ptr), stmtPtr(body_ptr));
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_BREAK, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        break_stmt* node = new break_stmt(row, col);
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_CONTINUE, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        continue_stmt* node = new continue_stmt(row, col);
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_RETURN, ReturnExp, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        expPtr val = nullptr;
        if (children[1]->ptr) {
            val = expPtr(static_cast<expr*>(children[1]->ptr.release()));
        }
        return_stmt* node = new return_stmt(row, col, std::move(val));
        res->set_node(nodePtr(node));
        return res;
    }),


    // Return Expressions
    rule(ReturnExp, {Exp}),
    rule(ReturnExp, {}),

    // Expressions (from your original set)
    rule(Exp, {LOrExp}),
    ruleAction(rule(LVal, {ID, LValTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        lval_expr *p = static_cast<lval_expr*>(children[1]->ptr.release());
        p->setIdAndReverseDim(children[0]->str_val);
        p->setLoc(row, col);
        res->set_node(lvalPtr(p));
        return res;
    }),
    ruleAction(rule(LValTail, {LBK, Exp, RBK, LValTail}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        lval_expr *p = static_cast<lval_expr*>(children[3]->ptr.release());
        expr *ptr = static_cast<expr*>(children[1]->ptr.release());
        p->addDim(expPtr(ptr));
        res->set_node(nodePtr(p));
        return res;
    }),
    ruleAction(rule(LValTail, {}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(-1, -1); //loc not used
        lval_expr *p = new lval_expr();
        res->set_node(nodePtr(p));
        return res;
    }),
    ruleAction(rule(PrimaryExp, {LPR, Exp, RPR}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        res->set_node(std::move(children[1]->ptr));
        return res;
    }),
    rule(PrimaryExp, {LVal}),
    ruleAction(rule(PrimaryExp, {NUM}), [](std::vector<parseInfoPtr>& children) {
        auto row = children[0]->location.first;
        auto col = children[0]->location.second;
        parseInfoPtr res = std::make_unique<parseInfo>(row, col);
        auto ptr = new int_literal(row, col, std::stoul(children[0]->str_val));
        res->set_node(nodePtr(ptr));
        return res;
    }),
    rule(Number, {NUM}),
    rule(UnaryExp, {PrimaryExp}),
    rule(UnaryExp, {ID, LPR, FuncRParams, RPR}),
    rule(UnaryExp, {ID, LPR, RPR}),
    ruleAction(rule(UnaryExp, {UnaryOp, UnaryExp}), unaryFactory()),
    rule(UnaryOp, {PLUS}),
    rule(UnaryOp, {MINUS}),
    rule(UnaryOp, {NOT}),
    rule(FuncRParams, {Exp, FuncRParamsTail}),
    rule(FuncRParamsTail, {COMMA, Exp, FuncRParamsTail}),
    rule(FuncRParamsTail, {}),

    rule(MulExp, {UnaryExp}),
    ruleAction(rule(MulExp, {MulExp, MulOp, UnaryExp}), binaryFactory()),
    rule(MulOp, {STAR}),
    rule(MulOp, {SLASH}),
    rule(MulOp, {PERCENT}),
    rule(AddExp, {MulExp}),
    ruleAction(rule(AddExp, {AddExp, AddOp, MulExp}), binaryFactory()),
    rule(AddOp, {PLUS}),
    rule(AddOp, {MINUS}),
    rule(RelExp, {AddExp}),
    ruleAction(rule(EqExp, {EqExp, EqOp, RelExp}), binaryFactory()),
    rule(RelOp, {LT}),
    rule(RelOp, {GT}),
    rule(RelOp, {LTE}),
    rule(RelOp, {GTE}),
    rule(EqExp, {RelExp}),
    ruleAction(rule(EqExp, {EqExp, EqOp, RelExp}), binaryFactory()),
    rule(EqOp, {EQ}),
    rule(EqOp, {NEQ}),
    rule(LAndExp, {EqExp}),
    ruleAction(rule(LAndExp, {LAndExp, AND, EqExp}), binaryFactory()),
    rule(LOrExp, {LAndExp}),
    ruleAction(rule(LOrExp, {LOrExp, OR, LAndExp}), binaryFactory()),
    rule(ConstExp, {Exp})
};



std::vector<rule> getBasicRules(const std::vector<ruleAction> &ps) {
    std::vector<rule> basicRules;
    for (const auto& ruleAction : ps) {
        basicRules.push_back(ruleAction.rule);
    }
    return basicRules;
}

const std::vector<rule> parserRules = getBasicRules(ruleWithAction);

std::set<symbolType> 
computeNonterminals(const std::vector<rule> &rules) 
{
    std::set<symbolType> nonterminals;
    for(const auto& rule : rules) {
        nonterminals.insert(rule.left);
    }
    return nonterminals;
}

void printNonterminals(const std::set<symbolType>& nonterminals) {
    std::cout << "Nonterminal symbols:\n";
    std::cout << "--------------------\n";
    for (const auto& nt : nonterminals) {
        auto it = symbolTypeNames.find(nt);
        if (it != symbolTypeNames.end()) {
            std::cout << it->second << "\n";
        } else {
            std::cout << "UNKNOWN_SYMBOL\n";
        }
    }
    std::cout << "--------------------\n";
    std::cout << "Total: " << nonterminals.size() << " nonterminals\n";
}

void printAllRules(const std::vector<rule>& rules) {
    std::cout << "Parser Rules (Grouped by Nonterminal):\n";
    std::cout << "=====================================\n\n";
    
    // First collect all nonterminals that appear on the left
    const std::set<symbolType> &nonterminals = computeNonterminals(rules);
    
    // Print rules for each nonterminal
    for (const symbolType& nt : nonterminals) {
        std::cout << symbolTypeNames.at(nt) << ":\n";
        
        int rule_num = 1;
        for (const auto& r : rules) {
            if (r.left == nt) {
                std::cout << "  " << rule_num++ << ") " << symbolTypeNames.at(r.left) << " → ";
                
                if (r.right.empty()) {
                    std::cout << "ε";
                } else {
                    for (size_t i = 0; i < r.right.size(); ++i) {
                        if (i > 0) std::cout << " ";
                        std::cout << symbolTypeNames.at(r.right[i]);
                    }
                }
                
                std::cout << "\n";
            }
        }
        std::cout << "\n";
    }
    
    std::cout << "=====================================\n";
    std::cout << "Total: " << rules.size() << " rules\n";
}

firstFollowSet 
computeFirstSet(const std::vector<rule> &rules) 
{
    const auto& nonterminals = computeNonterminals(rules);

    firstFollowSet res;
    for(auto nt : nonterminals) {
        res[nt] = {};
    }

    bool updated = true;
    while(updated) {
        updated = false;
        for(const auto &rule : rules) {
            const auto &lhs = rule.left;
            const auto &rhs = rule.right;
            if(rhs.size() == 0) {
                updated |= res[lhs].insert(EMPTY).second;
            } else if(nonterminals.count(rhs[0]) == 0) {
                updated |= res[lhs].insert(rhs[0]).second;
            } else {
                for(auto sym : rhs) {
                    for(auto t : res[sym]) {
                        updated |= res[lhs].insert(t).second;
                    }
                    if(res[sym].count(EMPTY) == 0) break;
                }
            }
        }
    }

    return res;
}

void 
printFirstOrFollowSets(const std::unordered_map<symbolType, 
    std::set<symbolType>> &Sets, bool first) 
{
    std::string str;
    if(first) str = "First"; else str = "Follow";

    std::cout << "Result of " + str + " Sets\n";
    std::cout << "=====================================\n\n";
    
    // Print rules for each nonterminal
    for (const auto &firstSetPair : Sets) {
        auto nt = firstSetPair.first;
        std::cout << symbolTypeNames.at(nt) << ":\n";
        
        int rule_num = 1;
        size_t i = 0;
        std::cout << "  " << rule_num++ << ") " + str + "(" << symbolTypeNames.at(nt) << ") = {";
        for (auto t : firstSetPair.second) {
            std::string str = symbolTypeNames.at(t);
            if(i != firstSetPair.second.size() - 1) str += ", ";
            i++;
            std::cout << str;
        }
        std::cout << "}\n\n";
    }
    
    std::cout << "=====================================\n";
    std::cout << "Total: " << Sets.size() << " " + str + " Sets\n\n";
}

firstFollowSet 
computeFollowSet(const std::vector<rule> &rules,
    const firstFollowSet &firstSet) 
{
    firstFollowSet res;
    const auto& nonterminals = computeNonterminals(rules);
    for(auto nt : nonterminals) {
        res[nt] = {};
    }
    bool updated = true;
    while(updated) {
        updated = false;
        for(const auto &rule : rules) {
            const auto &lhs = rule.left;
            const auto &rhs = rule.right;
            for(size_t i = 0; i < rhs.size(); i++) {
                if(nonterminals.count(rhs[i]) > 0) {
                    if(i != rhs.size() - 1) {
                        for(size_t j = i + 1; j < rhs.size(); j++) {
                            if(nonterminals.count(rhs[j]) > 0) {
                                for(auto sym : firstSet.at(rhs[j])) {
                                    if(sym != EMPTY) updated |= res[rhs[i]].insert(sym).second;
                                }
                                if(firstSet.at(rhs[j]).count(EMPTY) == 0) break;
                            } else {
                                updated |= res[rhs[i]].insert(rhs[j]).second;
                                break;
                            }
                        }
                    } else {
                        for(auto sym : res[lhs]) {
                            updated |= res[rhs[i]].insert(sym).second;
                        }
                    }
                }
            }
        }
    }
    return res;
} 

std::set<symbolType>
computeLookaheads(const rule &rule, size_t pos, symbolType la,
    const firstFollowSet &firstSet)
{
    const auto &nonterminals = computeNonterminals(parserRules);
    std::set<symbolType> lookaheads;
    for(size_t j = pos; j < rule.right.size() + 1; j++) {//
        if(j == rule.right.size()) {
            // std::cout << symbolTypeNames.at(la) << std::endl;
            lookaheads.insert(la);
            break;
        }
        if(nonterminals.count(rule.right[j]) == 0) {
            lookaheads.insert(rule.right[j]);
            break;
        }
        for(auto firstSym : firstSet.at(rule.right[j])) {
            if(firstSym != EMPTY) lookaheads.insert(firstSym);
        }
        if(firstSet.at(rule.right[j]).count(EMPTY) == 0) {
            break;
        }
    }

    return lookaheads;
}

std::set<lrItem> 
computeClosure(const std::set<lrItem> &coreItems,
    const firstFollowSet &firstSet) 
{
    
    std::set<lrItem> res = coreItems;
    bool updated = true;
    std::set<lrItem> workingList = coreItems;
    std::set<lrItem> newItems = {};
    const auto &nonterminals = computeNonterminals(parserRules); 
    //Y -> s.Xu (b)  => lookahead is first(ub) for X -> ....
                      
    while(updated) {
        // printLR1Items(workingList);
        // std::cout << "WORKINGLISTSIZE:" << workingList.size() << "\n";
        updated = false;
        for(const auto &item : workingList) {
            // if the right is not empty
            if(item.rule.right.size() > 0) {
                symbolType sym = item.rule.right[item.pos];
                // std::cout << symbolTypeNames.at(sym) << "FUCK" << item.pos << std::endl;
                if(nonterminals.count(sym) == 0) continue;
                for(size_t i = 0; i < parserRules.size(); i++) {
                    if(parserRules[i].left == sym) {
                        std::set<symbolType> lookaheads = computeLookaheads(item.rule, item.pos + 1, item.lookahead, firstSet);
                        for(auto la : lookaheads) {
                            lrItem newItem(parserRules[i], 0, la);
                            if(parserRules[i].left == LValTail) {
                                if(parserRules[i].right.size() == 0 && res.count(newItem) == 1) {
                                    auto it = res.find(newItem);
                                    while(1);
                                }
                            }
                            
                            if(res.insert(newItem).second) {
                                // std::cout << symbolTypeNames.at(newItem.rule.right[0]) << "FUCK" << std::endl;
                                newItems.insert(newItem);
                                updated = true;
                                
                            }
                        }
                    }
                }
            }
        }
        if(updated) {
            workingList.clear();
            workingList.insert(newItems.begin(), newItems.end());
            newItems.clear();
        }
    }
    return res;
}

void printLR1Items(const std::set<lrItem>& items) {
    if(items.empty()) return;
    std::cout << "Closure:\n";
    std::cout << "============\n";

    
    for (const auto& item : items) {
        // Print ruleuction
        std::cout << symbolTypeNames.at(item.rule.left) << " → ";
        
        // Print right-hand side with dot
        for (size_t i = 0; i < item.rule.right.size(); ++i) {
            if (i == item.pos) std::cout << "• ";
            std::cout << symbolTypeNames.at(item.rule.right[i]) << " ";
        }
        
        // Handle dot at end case
        if (item.pos == item.rule.right.size()) {
            std::cout << "•";
        }
        
        // Print lookahead symbols
        std::cout << ", " + symbolTypeNames.at(item.lookahead);
        
        std::cout << "\n";
    }
    
    std::cout << "============\n";
    std::cout << "Total: " << items.size() << " items\n";
}

lrState 
computeNextState(lrState state, symbolType edge, 
    const firstFollowSet &firstSet) 
{
    std::set<lrItem> items;
    for(const auto &item : state.items) {
        if(item.pos < item.rule.right.size()) {
            if(item.rule.right[item.pos] == edge) {
                items.insert(lrItem(item.rule, item.pos + 1, item.lookahead));
            }
        }
    }
    items = computeClosure(items, firstSet);
    return lrState(items);
}

LRTable 
computeLRTable(
    const firstFollowSet &firstSet,
    lrState start)
{
    
    std::vector<lrState> allState = {start};
    LRTable table; 
    table.push_back({});
    std::queue<std::pair<lrState, size_t>> workingQueue;
    const auto &nonterminals = computeNonterminals(parserRules);
    workingQueue.push({start, 0});
    std::map<rule, size_t> ruleToId;
    
    for(size_t i = 0; i < parserRules.size(); i++) {
        ruleToId[parserRules[i]] = i;
    }
    while(!workingQueue.empty()) {
        // std::cout << "HEY" << workingQueue.size() << "\n";
        const auto &curState = workingQueue.front().first;
        const size_t curId = workingQueue.front().second;
        // printLR1Items(curState.items);
        std::set<symbolType> edges;
        
        for(const auto &item : curState.items) {
            if(item.pos < item.rule.right.size()) {
                edges.insert(item.rule.right[item.pos]);
            } else { //reduce
                if(table[curId].count(item.lookahead) == 0) {
                    auto startSymbol = parserRules[0].left;
                    if(item.lookahead != END_OF_FILE || item.rule.left != startSymbol)
                        table[curId].insert({item.lookahead, action(actionType::REDUCE, ruleToId[item.rule])});
                    else
                        table[curId].insert({item.lookahead, action(actionType::ACC, ruleToId[item.rule])});
                } else {
                    std::cout << "conflict(1)! entry already exist! \n";
                    exit(0);
                }
            }
        }
        for(const auto &edge: edges) {
            const auto &newState = computeNextState(curState, edge, firstSet);
            bool isNew = true;
            size_t idx = -1;
            for(size_t i = 0; i < allState.size(); i++) {
                if(newState == allState[i]) {
                    isNew = false;
                    idx = i;
                    break;
                }
            }
            if(isNew) {
                idx = allState.size();
                allState.push_back(newState);
                workingQueue.push({newState, idx});
                table.push_back({});
            }
            if(table[curId].count(edge) == 0) {
                if(nonterminals.count(edge) > 0) { //goto
                    table[curId].insert({edge, action(actionType::GOTO, idx)});
                } else { //shift
                    table[curId].insert({edge, action(actionType::SHIFT, idx)});
                }
            } else {
                bool resolved = false;
                for(const auto &item : allState[curId].items) {
                    if(item.rule == rule(Stmt, {KW_IF, LPR, Exp, RPR, Stmt, KW_ELSE, Stmt}) 
                        && item.pos == 5 && edge == KW_ELSE) 
                    {
                        table[curId][edge] = action(actionType::SHIFT, idx);
                        resolved = true;
                        std::cout << "resolved: " << resolved << std::endl;
                    }
                }
                if(!resolved) {
                    std::cout << "conflict(2)! entry already exist! \n";
                    exit(0);
                }
   
            }
        }

        workingQueue.pop();
    }
    return table;
}


void exportLRTableToCSV(const LRTable& table, const std::string& filename) {
    std::ofstream csv(filename);
    if (!csv.is_open()) {
        std::cerr << "Failed to create CSV file\n";
        return;
    }

    // Collect all symbols
    std::set<symbolType> symbols;
    for (const auto& state : table) {
        for (const auto& entry : state) {
            symbols.insert(entry.first);
        }
    }

    

    // Header row
    csv << "State";
    for (const auto& sym : symbols) {
        csv << "," << symbolTypeNames.at(sym);
    }
    csv << "\n";

    // Data rows
    for (size_t state = 0; state < table.size(); ++state) {
        csv << state;
        for (const auto& sym : symbols) {
            csv << ",";
            auto it = table[state].find(sym);
            if (it != table[state].end()) {
                const action& a = it->second;
                switch (a.type) {
                    case SHIFT: csv << "s" << a.n; break;
                    case REDUCE: csv << "r" << a.n; break;
                    case GOTO: csv << a.n; break;
                    case ACC: csv << "acc"; break;
                }
            }
        }
        csv << "\n";
    }

    

    // Grammar productions sheet
    csv << "\n\nProductions\nIndex,Production\n";
    for (size_t i = 0; i < parserRules.size(); ++i) {
        csv << i << ",\"" << symbolTypeNames.at(parserRules[i].left) << " → ";
        for (const auto& sym : parserRules[i].right) {
            csv << symbolTypeNames.at(sym) << " ";
        }
        csv << "\"\n";
    }

    csv.close();
    std::cout << "Table saved to " << filename << " (open with Excel)\n";
}

parseResult
    Parse(std::vector<token> input, const LRTable &table, bool debug) 
{   
    std::vector<symbolType> str;
    for(const auto &tok : input) {
        str.push_back(tokenToSymbol(tok.type));
    }
    str.push_back(END_OF_FILE); // Add EOF marker
    std::vector<size_t> stateStack;
    stateStack.push_back(0); // Initial state
    std::vector<symbolType> symStack;

    std::vector<parseInfoPtr> infoStack; 

    std::vector<std::unique_ptr<parseTreeNode>> nodeStack;
    nodeStack.push_back(std::make_unique<parseTreeNode>(parserRules[0].left));

    size_t i = 0; // Input position
    size_t steps = 0;
    while(true) {
        size_t curState = stateStack.back();
        symbolType curSym = str[i];
        std::pair<size_t, size_t> location;
        if(i != input.size())
            location = input[i].location;
        else
            location = input.back().location;

        if (debug) {
            std::cout << "\nCurrent State: " << curState 
                      << ", Next Symbol: " << symbolTypeNames.at(curSym) << "\n";
            std::cout << "State Stack: [";
            for (auto s : stateStack) std::cout << s << " ";
            std::cout << "]\nSymbol Stack: [";
            for (auto s : symStack) std::cout << symbolTypeNames.at(s) << " ";
            std::cout << "]\nRemaining Input: [";
            for (size_t j = i; j < str.size() && j < i+5; j++)
                std::cout << symbolTypeNames.at(str[j]) << " ";
            std::cout << "...]\n";
        }


        if(table[curState].count(curSym)) {
            const action &a = table[curState].at(curSym);

            switch (a.type) {
                case SHIFT: {
                    if (debug) {
                        std::cout << "ACTION: Shift to state " << a.n 
                                  << " with symbol '" << symbolTypeNames.at(curSym) 
                                  << " at (" << location.first << ", " << location.second << ")" 
                                  << "'\n";
                    }
                    auto node = std::make_unique<parseTreeNode>(
                        curSym
                    );
                    nodeStack.push_back(std::move(node));

                    const auto &curTok = input[i];

                    parseInfoPtr ptr = std::make_unique<parseInfo>(curTok.location.first, curTok.location.second);

                    ptr->set_str(curTok.value);
                    ptr->set_type(curSym);
                    ptr->set_node(nullptr);

                    infoStack.push_back(std::move(ptr));

                    stateStack.push_back(a.n);
                    symStack.push_back(curSym);
                    i++;
                    break;
                }
                case REDUCE: {
                    const rule &r = parserRules[a.n];
                    
                    if (debug) {
                        std::cout << "ACTION: Reduce by rule " << a.n << " ("
                                  << symbolTypeNames.at(r.left) << " → ";
                        for (const auto& sym : r.right) {
                            std::cout << symbolTypeNames.at(sym) << " ";
                        }
                        std::cout << ")\n";
                    }

                    // Check stack underflow
                    if (symStack.size() < r.right.size()) {
                        std::cerr << "PARSE ERROR: Cannot reduce rule " << a.n 
                                  << " - not enough symbols on stack\n";
                        return parseResult{
                                    .parseTree = nullptr,
                                    .node = nullptr
                                };
                    }

                    // Pop right-hand side
                    auto parent = std::make_unique<parseTreeNode>(r.left);
                    std::vector<parseInfoPtr> RHSChildren;
                    for (size_t j = 0; j < r.right.size(); j++) {
                        parent->addChild(std::move(nodeStack.back()));
                        nodeStack.pop_back();
                        symStack.pop_back();
                        stateStack.pop_back();
                        RHSChildren.push_back(std::move(infoStack.back()));
                        infoStack.pop_back();
                    }

                    std::reverse(RHSChildren.begin(), RHSChildren.end());
                    

                    infoStack.push_back(ruleWithAction[a.n].action(RHSChildren));
                    
                    // std::cout << "infoStack size:" << infoStack.size() << std::endl;
                    auto ptr = infoStack.back().get();
                    if(ptr != nullptr) {
                        if(ptr->ptr != nullptr) {
                            ptr->ptr->printAST("", "");
                        }
                    }

                    std::reverse(parent->children.begin(), parent->children.end());
                    nodeStack.push_back(std::move(parent));

                    // Push left-hand side
                    symStack.push_back(r.left);

                    // Goto new state
                    curState = stateStack.back();
                    if (table[curState].count(r.left) == 0) {
                        std::cerr << "PARSE ERROR: No GOTO for state " << curState 
                                  << " and nonterminal '" << symbolTypeNames.at(r.left) << "'\n";
                        return parseResult{
                                    .parseTree = nullptr,
                                    .node = nullptr
                                };
                    }
                    size_t newState = table[curState].at(r.left).n;
                    stateStack.push_back(newState);

                    if (debug) {
                        std::cout << "GOTO state " << newState << " after reduction\n";
                    }
                    break;
                }
                case ACC: {
                    if(curSym == END_OF_FILE) { //eof
                        if (debug) {
                            std::cout << "ACTION: Accept - parsing successful in " << steps << " steps!\n";
                        }
                        std::cout << "number of ast:" << infoStack.size() << std::endl;
                        return parseResult{
                                    .parseTree = std::move(nodeStack.back()),
                                    .node = std::move(infoStack.back()->ptr)
                                };
                    } else {
                        if (debug) {
                            std::cout << "opps, there are some unread text:" << symbolTypeNames.at(curSym) << " \n";
                        }
                        return parseResult{
                                    .parseTree = nullptr,
                                    .node = nullptr
                                };
                    }
                }
                case GOTO: {
                    std::cerr << "PARSE ERROR: Unexpected GOTO action\n";
                    return parseResult{
                                .parseTree = nullptr,
                                .node = nullptr
                            };
                }
            }
        } else {
            std::cerr << "PARSE ERROR: No action defined for state " << curState 
                      << " and symbol '" << symbolTypeNames.at(curSym) 
                      << " at (" << location.first << ", " << location.second << ")" 
                      << "'\n";
            
            if (debug) {
                std::cerr << "Current State Stack: [";
                for (auto s : stateStack) std::cerr << s << " ";
                std::cerr << "]\nSymbol Stack: [";
                for (auto s : symStack) std::cerr << symbolTypeNames.at(s) << " ";
                std::cerr << "]\nNext 5 Input Symbols: [";
                for (size_t j = i; j < str.size() && j < i+5; j++)
                    std::cerr << symbolTypeNames.at(str[j]) << " ";
                std::cerr << "]\n";
            }
            
            return parseResult{
                                .parseTree = nullptr,
                                .node = nullptr
                            };
        }
        steps++;
    }
}

void visualizeAsTree(const parserTreePtr &node, 
                    const std::string& prefix, 
                    bool isLast){
    if(node == nullptr) return;
    std::cout << prefix;
    std::cout << (isLast ? "└──" : "├──");
    
    std::cout << symbolTypeNames.at(node->type);
    std::cout << "\n";

    for (size_t i = 0; i < node->children.size(); i++) {
        bool lastChild = (i == node->children.size() - 1);
        visualizeAsTree(node->children[i], 
                       prefix + (isLast ? "    " : "│   "),
                       lastChild);
    }
}

LRTable importLRTableFromCSV(const std::string& filename) {
    std::ifstream csv(filename);
    if(!csv.is_open()) {
        throw std::runtime_error("Failed to open CSV file");
    }
    std::vector<symbolType> symbols;
    std::map<std::string, symbolType> nameToSym;
    for(auto &pair : symbolTypeNames) {
        nameToSym[pair.second] = pair.first;
    }

    LRTable table;

    std::string line;
    std::getline(csv, line);

    std::stringstream ss(line);
    std::string cell;
    
    // Skip first column (State)
    std::getline(ss, cell, ',');
    
    while (std::getline(ss, cell, ',')) {
        if (nameToSym.find(cell) == nameToSym.end()) {
            throw std::runtime_error("Unknown symbol in CSV: " + cell);
        }
        symbols.push_back(nameToSym[cell]);
    }

    while(std::getline(csv, line)) {
        if(line.empty() || line.find("Productions") != std::string::npos) {
            break;
        }
        std::stringstream ss(line);
        std::string cell;


        std::unordered_map<symbolType, action> actionMap;

        // Skip first column (State)
        std::getline(ss, cell, ',');

        for(size_t i = 0; i < symbols.size(); i++) {
            std::getline(ss, cell, ',');
            
            if(cell.empty()) continue;

            if(cell[0] == 'r') {
                actionMap[symbols[i]] = action(actionType::REDUCE, std::stoul(cell.substr(1)));
            } else if(cell[0] == 's') {
                actionMap[symbols[i]] = action(actionType::SHIFT, std::stoul(cell.substr(1)));
            } else if(cell == "acc") {
                actionMap[symbols[i]] = action(actionType::ACC, 0);
            } else {
                actionMap[symbols[i]] = action(actionType::GOTO, std::stoul(cell));
            }
        }
        table.push_back(actionMap);
    }
    return table;
}