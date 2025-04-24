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
        case token::POINTER_ACC: return POINTER_ACC;

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
        case token::KW_CONST: return KW_CONST;
        case token::KW_BREAK: return KW_BREAK;
        case token::KW_CONTINUE: return KW_CONTINUE;

        // Special
        case token::END_OF_FILE: return END_OF_FILE;
        case token::INVALID: return INVALID;

        default:
            std::cout << "unrecognized token at tokenToSymbol\n";
            exit(1);
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
    {POINTER_ACC, "POINTER_ACC"},

    // Keywords
    {KW_IF, "KW_IF"},
    {KW_ELSE, "KW_ELSE"},
    {KW_WHILE, "KW_WHILE"},
    {KW_FOR, "KW_FOR"},
    {KW_RETURN, "KW_RETURN"},
    {KW_FUNCTION, "KW_FUNCTION"},
    {KW_CLASS, "KW_CLASS"},
    {KW_CONTINUE, "KW_CONTINUE"},
    {KW_BREAK, "KW_BREAK"},

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
    {ConstExp, "ConstExp"},
    {ClassDef, "ClassDef"},
    {ClassBody, "ClassBody"},
    {ClassMember, "ClassMember"},
    {ClassMemberList, "ClassMemberList"},
    {ConstructorDef, "ConstructorDef"},

    {Declarator,"Declarator"}, 
    {SimpleDeclarator, "SimpleDeclarator"}, 
    {DirectDeclarator, "DirectDeclarator"},
    {TypeSuffix, "TypeSuffix"}, 
    {TypeDeclIdxTail, "TypeDeclIdxTail"}
};

const auto initLoc = std::pair<size_t, size_t>{-1, -1};

SemanticAction binaryFactory() {
    return [](std::vector<parseInfoPtr>& children) {
        // std::cout << "fuck\n";
        auto left = expPtr(static_cast<expr*>(children[0]->ptr.release()));
        auto right = expPtr(static_cast<expr*>(children[2]->ptr.release()));
        auto ptr = new binary_expr(left->location, operatorSymbols.at(children[1]->kind), 
                                    std::move(left), std::move(right));
        parseInfoPtr res = std::make_unique<parseInfo>(ptr->location);
        res->set_node(nodePtr(ptr));
        return res;
    };
}

SemanticAction unaryFactory() {
    return [](std::vector<parseInfoPtr>& children) {
        auto operand = expPtr(static_cast<expr*>(children[1]->ptr.release()));
        auto ptr = new unary_expr(children[0]->location, operatorSymbols.at(children[0]->kind), 
                                    std::move(operand));
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        res->set_node(nodePtr(ptr));
        return res;
    };
}

SemanticAction compUnitFactory() {
    return [](std::vector<parseInfoPtr> &children) -> parseInfoPtr {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto prog = static_cast<program*>(children[0]->ptr.release());
        prog->children.push_back(children[1]->ptr.release());
        res->set_node(nodePtr(prog));
        return res;
    };
}

SemanticAction compUnitFactory_1() {
    return [](std::vector<parseInfoPtr> &children) -> parseInfoPtr {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto prog = new program(children[0]->location);
        prog->children.push_back(children[0]->ptr.release());
        res->set_node(nodePtr(prog));
        return res;
    };
}

// SemanticAction funcDefFactory() {
//     return [](std::vector<parseInfoPtr>& children) {
//         parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);

//         // Create a new function definition with the specified return type and name
//         auto funcDef = static_cast<func_def*>(children[3]->ptr.release());
//         funcDef->set_ret_type(children[0]->str_val);
//         funcDef->set_id_reverse_params(children[1]->str_val);

//         funcDef->setLoc(children[0]->location);
//         // funcDef->evaluateType();//test
//         // Set the function body (block)
//         funcDef->set_body(blockPtr(static_cast<block_stmt*>(children[5]->ptr.release())));
//         res->set_node(funcDefPtr(funcDef));
//         return res;
//     };
// }

SemanticAction varDefFactory() {
    return [](std::vector<parseInfoPtr> &children) -> parseInfoPtr {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto tail = static_cast<var_decl*>(children[2]->ptr.release());
        tail->addVarDef(vardefPtr(static_cast<var_def*>(children[1]->ptr.release())));
        res->set_node(nodePtr(tail));
        return res;
    };
}
const std::vector<ruleAction> ruleWithAction = {
    // Start Symbol
    rule(START, {CompUnit}),

    // Program Structure
    ruleAction(rule(CompUnit, {Decl}), compUnitFactory_1()),
    ruleAction(rule(CompUnit, {FuncDef}), compUnitFactory_1()),
    ruleAction(rule(CompUnit, {CompUnit, Decl}), compUnitFactory()),
    ruleAction(rule(CompUnit, {CompUnit, FuncDef}), compUnitFactory()),

    ruleAction(rule(CompUnit, {ClassDef}), compUnitFactory_1()),
    ruleAction(rule(CompUnit, {CompUnit, ClassDef}), compUnitFactory()),

    ruleAction(rule(ClassDef, {KW_CLASS, ID, LBC, ClassBody, RBC, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto classDef = static_cast<class_def*>(children[3]->ptr.release());
        classDef->setName(children[1]->str_val);
        classDef->setLoc(children[0]->location);
        classDef->reverseChildren();
        res->set_node(nodePtr(classDef));
        return res;
    }),
    ruleAction(rule(ClassBody, {}), [](std::vector<parseInfoPtr>&) {
        parseInfoPtr res = std::make_unique<parseInfo>(initLoc);
        res->set_node(std::make_unique<class_def>(initLoc)); // Empty class_def
        return res;
    }),
    
    rule(ClassBody, {ClassMemberList}), // class with members


    ruleAction(rule(ClassMemberList, {ClassMember}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto def = std::make_unique<class_def>(children[0]->location);
        def->addChild(std::move(children[0]->ptr));
        res->set_node(std::move(def));
        return res;
    }),

    // ClassMemberList → ClassMember ClassMemberList
    ruleAction(rule(ClassMemberList, {ClassMember, ClassMemberList}), [](std::vector<parseInfoPtr>& children) {
        auto def = static_cast<class_def*>(children[1]->ptr.release());
        def->addChild(std::move(children[0]->ptr));
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        res->set_node(nodePtr(def));
        return res;
    }),

    rule(ClassMember, {VarDecl}),       // member variable
    rule(ClassMember, {FuncDef}),       // member function
    rule(ClassMember, {ConstructorDef}), // optional: constructor

    ruleAction(rule(ConstructorDef, {ID, LPR, FuncFParams, RPR, Block}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto fun = static_cast<FuncType*>(children[2]->type);
        auto funcDef = new func_def(children[0]->location);
        funcDef->type = fun;
        funcDef->name = children[0]->str_val;
        funcDef->set_body(blockPtr(static_cast<block_stmt*>(children[4]->ptr.release())));
        funcDef->setCtor();
        
        res->set_node(nodePtr(funcDef));
        return res;
    }),// optional: constructor

    ruleAction(rule(ConstructorDef, {ID, LPR, RPR, Block}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto funcDef = new func_def(children[0]->location);
        auto fun = new FuncType();
        funcDef->set_body(blockPtr(static_cast<block_stmt*>(children[3]->ptr.release())));
        funcDef->setCtor();
        funcDef->name = children[0]->str_val;
        funcDef->type = fun;
        res->set_node(nodePtr(funcDef));
        return res;
    }),

    // Declarations
    rule(Decl, {ConstDecl}),
    rule(Decl, {VarDecl}),

    // Elementary Types
    rule(Type, {KW_INT}),
    rule(Type, {KW_VOID}),
    ruleAction(rule(Type, {KW_CLASS, ID}), [](std::vector<parseInfoPtr>& children) {
        return std::move(children[1]);
    }), 
    // Constant Declarations
    // ruleAction(rule(ConstDecl, {KW_CONST, Type, ConstDef, ConstDeclTail, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
    //     parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
    //     auto tail = static_cast<var_decl*>(children[3]->ptr.release());
    //     tail->addVarDef(vardefPtr(static_cast<var_def*>(children[2]->ptr.release())));
    //     tail->setTypeAndReverse(children[1]->str_val);
    //     // std::cout << "je\n";
    //     res->set_node(nodePtr(tail));
    //     tail->setLoc(children[0]->location);
    //     return res;
    // }),
    // ruleAction(rule(ConstDeclTail, {COMMA, ConstDef, ConstDeclTail}), varDefFactory()),
    // ruleAction(rule(ConstDeclTail, {}), [](std::vector<parseInfoPtr>&) {
    //     auto res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{-1, -1});
    //     auto varDecl = std::make_unique<var_decl>(std::pair<size_t, size_t>{-1, -1});
    //     varDecl->setConst(true);
    //     res->set_node(std::move(varDecl));
    //     return res;
    // }),

    // // Variable Declarations
    ruleAction(rule(VarDecl, {Type, VarDef, VarDeclTail, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto tail = static_cast<var_decl*>(children[2]->ptr.release());
        tail->addVarDef(vardefPtr(static_cast<var_def*>(children[1]->ptr.release())));
        tail->setTypeAndReverse(children[0]->str_val);
        res->set_node(nodePtr(tail));
        tail->setLoc(children[0]->location);
        return res;
    }),
    ruleAction(rule(VarDeclTail, { COMMA, VarDef, VarDeclTail}), varDefFactory()),

    ruleAction(rule(VarDeclTail, {}), [](std::vector<parseInfoPtr>&) {
        auto res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{std::pair<size_t, size_t>{-1, -1}});
        auto varDecl = std::make_unique<var_decl>(std::pair<size_t, size_t>{std::pair<size_t, size_t>{-1, -1}});
        res->set_node(std::move(varDecl));
        return res;
    }),

    // Definitions
    // ruleAction(rule(ConstDef, {ID, ConstDefTail, ASSIGN, ConstInitVal}), [](std::vector<parseInfoPtr>& children) {
    //     parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);

    //     std::string id = children[0]->str_val;
    //     var_def* arr = static_cast<var_def*>(children[1]->ptr.release());
    //     arr->setId(id);
    //     arr->setInitVal(nodePtr(children[3]->ptr.release()));
    //     res->set_node(nodePtr(arr));
    //     return res;
    // }),
    // ruleAction(rule(ConstDefTail, {LBK, ConstExp, RBK, ConstDefTail}), [](std::vector<parseInfoPtr>& children) {
        
    //     parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
    //     auto tail = static_cast<var_def*>(children[3]->ptr.release());
    //     expr *ptr = static_cast<expr*>(children[1]->ptr.release());
    //     tail->addDim(expPtr(ptr));
    //     res->set_node(nodePtr(tail));
    //     return res;
    // }),

    // ruleAction(rule(ConstDefTail, {}),  [](std::vector<parseInfoPtr>&) {
    //     auto res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{-1, -1});
    //     auto varDef = std::make_unique<var_def>(std::pair<size_t, size_t>{-1, -1});
    //     res->set_node(std::move(varDef));
    //     return res;
    // }),

    ruleAction(rule(VarDef, {Declarator}),[](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        std::string id = children[0]->str_val;
        var_def* arr = new var_def(children[0]->location);
        arr->setId(id);
        arr->type = children[0]->type;
        res->set_node(nodePtr(arr));
        return res;
    }),
    ruleAction(rule(VarDef, {Declarator, ASSIGN, InitVal}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        std::string id = children[0]->str_val;
        var_def* arr = new var_def(children[0]->location);
        arr->setId(id);
        arr->setInitVal(nodePtr(children[2]->ptr.release()));
        arr->type = children[0]->type;
        res->set_node(nodePtr(arr));
        return res;
    }),
    // ruleAction(rule(VarDefGroup, {LBK, ConstExp, RBK, VarDefGroup}), [](std::vector<parseInfoPtr>& children) {
        
    //     parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
    //     auto tail = static_cast<var_def*>(children[3]->ptr.release());
    //     expr *ptr = static_cast<expr*>(children[1]->ptr.release());
    //     tail->addDim(expPtr(ptr));
    //     res->set_node(nodePtr(tail));
    //     return res;
    // }),
    // ruleAction(rule(VarDefGroup, {}), [](std::vector<parseInfoPtr>&) {
    //     auto res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{-1, -1});
    //     auto varDef = std::make_unique<var_def>(std::pair<size_t, size_t>{-1, -1});
    //     res->set_node(std::move(varDef));
    //     return res;
    // }),

    // // Initialization
    // rule(ConstInitVal, {ConstExp}),
    // rule(ConstInitVal, {LBC, ConstInitValTail, RBC}),
    // rule(ConstInitValTail, {ConstInitVal, ConstInitValTailTail}),
    // rule(ConstInitValTail, {}),
    // rule(ConstInitValTailTail, {COMMA, ConstInitVal, ConstInitValTailTail}),
    // rule(ConstInitValTailTail, {}),

    ruleAction(rule(InitVal, {Exp}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);

        auto val = std::make_unique<init_val>(children[0]->location);
        val->setScalar(expPtr(static_cast<expr*>(children[0]->ptr.release())));
        val->setLoc(children[0]->location);
        res->set_node(std::move(val));
        return res;
    }),
    ruleAction(rule(InitVal, {LBC, InitValTail, RBC}), [](std::vector<parseInfoPtr>& children) {
        children[1]->ptr->location = children[0]->location;
        static_cast<init_val*>(children[1]->ptr.get())->reverseChildren();
        return std::move(children[1]);
    }),
    ruleAction(rule(InitValTail, {InitVal, InitValTailTail}), [](std::vector<parseInfoPtr>& children) {
        auto container = static_cast<init_val*>(children[1]->ptr.get());
        container->addChild(initValPtr(static_cast<init_val*>(children[0]->ptr.release())));
        return std::move(children[1]);
    }),
    ruleAction(rule(InitValTail, {}), [](std::vector<parseInfoPtr>&) {
        auto res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{-1, -1});
        res->set_node(std::make_unique<init_val>(std::pair<size_t, size_t>{-1, -1}));
        return res;
    }),
    ruleAction(rule(InitValTailTail, {COMMA, InitVal, InitValTailTail}), [](std::vector<parseInfoPtr>& children) {
        auto container = static_cast<init_val*>(children[2]->ptr.get());
        init_val *p = static_cast<init_val*>(children[1]->ptr.release());
        p->setLoc(children[0]->location);
        container->addChild(initValPtr(p));
        return std::move(children[2]);
    }),
    ruleAction(rule(InitValTailTail, {}), [](std::vector<parseInfoPtr>&) {
        auto res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{-1, -1});
        res->set_node(std::unique_ptr<init_val>(new init_val(std::pair<size_t, size_t>{-1, -1})));
        return res;
    }),

    ruleAction(rule(FuncDef, {Type, Declarator, Block}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto funcDef = new func_def(children[0]->location);
        funcDef->name = children[1]->str_val;
        funcDef->set_body(blockPtr(static_cast<block_stmt*>(children[2]->ptr.release())));
        if(children[1]->type == nullptr || children[1]->type->kind != TypeKind::Function) {
            std::cout << "funcdef error, not a function type\n";
            exit(1);
            return res;
        } 
        FuncType *fun = dynamic_cast<FuncType*>(children[1]->type);
        if(fun->retType == nullptr) {
            fun->retType = TypeFactory::getTypeFromName(children[0]->str_val);
        } else if(fun->retType->kind == TypeKind::Pointer) {
            PointerType *pointer = dynamic_cast<PointerType*>(fun->retType);
            pointer->elementType = TypeFactory::getTypeFromName(children[0]->str_val);
        }
        funcDef->type = fun;
        std::reverse(funcDef->type->argTypeList.begin(), funcDef->type->argTypeList.end());
        std::reverse(funcDef->type->bindings.begin(), funcDef->type->bindings.end());
        res->set_node(nodePtr(funcDef));
        return res;
    }),

    ruleAction(rule(Declarator, {STAR, Declarator}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        res->set_str(children[0]->str_val);
        struct Type *type = children[1]->type;
        if(type == nullptr) {
            children[1]->type = new PointerType();
        } else if(type->kind == TypeKind::Array) {
            ArrayType *arr = dynamic_cast<ArrayType*>(type);
            if(arr->element_type == nullptr) {
                PointerType *pointer = new PointerType();
                pointer->elementType = nullptr;
                arr->element_type = pointer;
            } else if(arr->element_type->kind == TypeKind::Pointer && 
                    ((PointerType*)(arr->element_type))->elementType == nullptr) {
                auto ptr = (PointerType*)(arr->element_type);
                ptr->depth++;
            } else {
                std::cout << "Pointer Fault\n";
                while(1);
            }
        } else if(type->kind == TypeKind::Function) {
            FuncType *fun = dynamic_cast<FuncType*>(type);
            if(fun->retType == nullptr) {
                PointerType *pointer = new PointerType();
                pointer->elementType = nullptr;
                fun->retType = pointer;
            } else if(fun->retType->kind == TypeKind::Pointer && 
                    ((PointerType*)(fun->retType))->elementType == nullptr) {
                auto ptr = (PointerType*)(fun->retType);
                ptr->depth++;
            } else {
                std::cout << "Pointer Fault\n";
                while(1);
            }
        } else if(type->kind == TypeKind::Pointer) {
            PointerType *pointer = dynamic_cast<PointerType*>(type);
            pointer->depth++;
        }
        res->set_type(children[1]->type);
        res->set_str(children[1]->str_val);
        return res;
    }),

    rule(Declarator, {DirectDeclarator}),

    ruleAction(rule(DirectDeclarator, {SimpleDeclarator, TypeSuffix}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        res->set_str(children[0]->str_val);
        res->set_type(children[1]->type);
        return res;
    }),

    ruleAction(rule(DirectDeclarator, {SimpleDeclarator}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        res->set_str(children[0]->str_val);
        res->set_type(nullptr);
        return res;
    }),

    rule(TypeSuffix, {TypeDeclIdxTail}),

    ruleAction(rule(TypeDeclIdxTail, {LBK, ConstExp, RBK, TypeDeclIdxTail}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        ArrayType *type = dynamic_cast<ArrayType*>(children[3]->type);
        type->addDim(expPtr(dynamic_cast<expr*>(children[1]->ptr.release())));
        res->set_type(type);
        return res;
    }),

    ruleAction(rule(TypeDeclIdxTail, {LBK, RBK, TypeDeclIdxTail}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        ArrayType *type = dynamic_cast<ArrayType*>(children[2]->type);
        type->addDim(expPtr(new int_literal(children[0]->location, 0)));
        res->set_type(type);
        return res;
    }),

    ruleAction(rule(TypeDeclIdxTail, {LBK, ConstExp, RBK}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        ArrayType *type = new ArrayType();
        type->addDim(expPtr(dynamic_cast<expr*>(children[1]->ptr.release())));
        res->set_type(type);
        return res;
    }),

    ruleAction(rule(TypeSuffix, {LPR, FuncFParams, RPR}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        res->set_type(children[1]->type);
        return res;
    }),

    ruleAction(rule(TypeSuffix, {LPR, RPR}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        FuncType *fun = new FuncType();
        res->set_type(fun);
        return res;
    }),

    ruleAction(rule(FuncFParams, {FuncFParam}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        FuncType *fun = new FuncType();
        fun->addArgType(children[0]->type);
        fun->bindings.push_back(children[0]->str_val);
        res->set_type(fun);
        return res;
    }),

    ruleAction(rule(FuncFParams, {FuncFParam, FuncFParamsTail}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        FuncType *fun = dynamic_cast<FuncType*>(children[1]->type);
        fun->addArgType(children[0]->type);
        fun->bindings.push_back(children[0]->str_val);
        res->set_type(fun);
        return res;
    }),

    ruleAction(rule(FuncFParamsTail, {COMMA, FuncFParam, FuncFParamsTail}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        FuncType *fun = dynamic_cast<FuncType*>(children[2]->type);
        fun->addArgType(children[1]->type);
        fun->bindings.push_back(children[1]->str_val);
        res->set_type(fun);
        return res;
    }),

    ruleAction(rule(FuncFParamsTail, {COMMA, FuncFParam}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        FuncType *fun = new FuncType();
        fun->addArgType(children[1]->type);
        fun->bindings.push_back(children[1]->str_val);
        res->set_type(fun);
        return res;
    }),

    ruleAction(rule(FuncFParam, {Type, Declarator}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        struct Type *result_type = TypeFactory::getTypeFromName(children[0]->str_val);
        if(children[1]->type == nullptr) { //no declarator
            children[1]->type = result_type;
        } else if(children[1]->type->kind == TypeKind::Function) {
            struct Type *type = children[1]->type;
            FuncType *func = dynamic_cast<FuncType*>(type);
            if(func->retType == nullptr) { //not a pointer
                func->retType = result_type;
            } else if(func->retType->kind == TypeKind::Pointer) {
                PointerType *pointer = dynamic_cast<PointerType*>(func->retType);
                pointer->elementType = result_type;
            } else {
                std::cout << "rule funcparam->type declarator\n";
                exit(1);
            }
        } else if(children[1]->type->kind == TypeKind::Array) {
            struct Type *type = children[1]->type;
            ArrayType *arr = dynamic_cast<ArrayType*>(type);
            if(arr->element_type == nullptr) { //not a pointer
                arr->element_type = result_type;
            } else if(arr->element_type->kind == TypeKind::Pointer) {
                PointerType *pointer = dynamic_cast<PointerType*>(arr->element_type);
                pointer->elementType = result_type;
            } else {
                std::cout << "rule funcparam->type declarator\n";
                exit(1);
            }
        } else if(children[1]->type->kind == TypeKind::Pointer) {
            dynamic_cast<PointerType*>(children[1]->type)->elementType = result_type;
        }
        res->set_type(children[1]->type);
        res->str_val = children[1]->str_val;
        return res;
    }),

    rule(SimpleDeclarator, {ID}),

    // Blocks and Statements
    ruleAction(rule(Block, {LBC, BlockItemTail, RBC}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);

        if (children[1] && children[1]->ptr) {
            block_stmt* blk = static_cast<block_stmt*>(children[1]->ptr.get());
            blk->setLoc(children[0]->location);
            std::reverse(blk->items.begin(), blk->items.end());  // Reverse here!
            res->set_node(std::move(children[1]->ptr));
        } else {
            res->set_node(std::make_unique<block_stmt>(children[0]->location));
        }

        return res;
    }),

    ruleAction(rule(BlockItemTail, {BlockItem, BlockItemTail}), [](std::vector<parseInfoPtr>& children) {
        

        auto* blk = static_cast<block_stmt*>(children[1]->ptr.get());
        if (children[0]->ptr) {
            blk->add_item(std::move(children[0]->ptr));
        }

        auto res = std::make_unique<parseInfo>(children[0]->location);
        res->set_node(std::move(children[1]->ptr));  // Return same block_stmt upwards
        return res;
    }),

    ruleAction(rule(BlockItemTail, {}), [](std::vector<parseInfoPtr>&) {
        auto res = std::make_unique<parseInfo>(std::pair<size_t,size_t>(0, 0));
        auto blk = std::make_unique<block_stmt>(std::pair<size_t,size_t>(0, 0));
        res->set_node(std::move(blk));
        return res;
    }),

    rule(BlockItem, {Decl}),
    rule(BlockItem, {Stmt}),

    // Statements
    rule(Stmt, {LVal, ASSIGN, Exp, SEMICOLON}),
    ruleAction(rule(Stmt, {Exp, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        expr *ptr = static_cast<expr*>(children[0]->ptr.release());
        expr_stmt* p = new expr_stmt(res->location, expPtr(ptr));
        res->set_node(nodePtr(p));
        return res;
    }),
    rule(Stmt, {SEMICOLON}),
    rule(Stmt, {Block}),
    
    ruleAction(rule(Stmt, {KW_IF, LPR, Exp, RPR, Stmt}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);

        expr* cond_ptr = static_cast<expr*>(children[2]->ptr.release());
        stmt* if_stmt_ptr = static_cast<stmt*>(children[4]->ptr.release());

        if_else_stmt* node = new if_else_stmt(res->location, expPtr(cond_ptr), stmtPtr(if_stmt_ptr));
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_IF, LPR, Exp, RPR, Stmt, KW_ELSE, Stmt}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);

        expr* cond_ptr = static_cast<expr*>(children[2]->ptr.release());
        stmt* if_stmt_ptr = static_cast<stmt*>(children[4]->ptr.release());
        stmt* else_stmt_ptr = static_cast<stmt*>(children[6]->ptr.release());

        if_else_stmt* node = new if_else_stmt(res->location, expPtr(cond_ptr), stmtPtr(if_stmt_ptr), stmtPtr(else_stmt_ptr));
        res->set_node(nodePtr(node));
        return res;
    }),

    ruleAction(rule(Stmt, {KW_WHILE, LPR, Exp, RPR, Stmt}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);

        expr* cond_ptr = static_cast<expr*>(children[2]->ptr.release());
        stmt* body_ptr = static_cast<stmt*>(children[4]->ptr.release());

        while_stmt* node = new while_stmt(res->location, expPtr(cond_ptr), stmtPtr(body_ptr));
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_BREAK, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        break_stmt* node = new break_stmt(res->location);
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_CONTINUE, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        continue_stmt* node = new continue_stmt(children[0]->location);
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_RETURN, Exp, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        expPtr val = nullptr;
        if (children[1]->ptr) {
            val = expPtr(static_cast<expr*>(children[1]->ptr.release()));
        }
        return_stmt* node = new return_stmt(children[0]->location, std::move(val));
        res->set_node(nodePtr(node));
        return res;
    }),
    ruleAction(rule(Stmt, {KW_RETURN, SEMICOLON}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        return_stmt* node = new return_stmt(children[0]->location);
        res->set_node(nodePtr(node));
        return res;
    }),

    // Expressions (from your original set)
    rule(Exp, {LOrExp}),
    // ruleAction(rule(LVal, {ID, LValTail}), [](std::vector<parseInfoPtr>& children) {
        
    //     parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
    //     lval_expr *p = static_cast<lval_expr*>(children[1]->ptr.release());
    //     p->setIdAndReverseDim(children[0]->str_val);
    //     p->setLoc(children[0]->location);
    //     res->set_node(lvalPtr(p));
    //     return res;
    // }),
    // ruleAction(rule(LValTail, {LBK, Exp, RBK, LValTail}), [](std::vector<parseInfoPtr>& children) {
        
    //     parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
    //     lval_expr *p = static_cast<lval_expr*>(children[3]->ptr.release());
    //     expr *ptr = static_cast<expr*>(children[1]->ptr.release());
    //     p->addDim(expPtr(ptr));
    //     res->set_node(nodePtr(p));
    //     return res;
    // }),
    ruleAction(rule(LValTail, {}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{-1, -1}); //loc not used
        lval_expr *p = new lval_expr();
        res->set_node(nodePtr(p));
        return res;
    }),
    ruleAction(rule(PrimaryExp, {LPR, Exp, RPR}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        res->set_node(std::move(children[1]->ptr));
        return res;
    }),
    rule(PrimaryExp, {LVal}),
    ruleAction(rule(PrimaryExp, {NUM}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        int_literal* ptr = new int_literal(children[0]->location, std::stoul(children[0]->str_val));
        res->set_node(nodePtr(ptr));
        return res;
    }),


    rule(Number, {NUM}),
    rule(UnaryExp, {PrimaryExp}),

    // pointer access
    ruleAction(rule(Exp, {Exp, POINTER_ACC, ID}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        expr *exp = dynamic_cast<expr*>(children[0]->ptr.release());
        auto ma = new pointer_acc(children[0]->location, expPtr(exp), children[2]->str_val);
        res->set_node(nodePtr(ma));
        return res;
    }),
    ruleAction(rule(Exp, {Exp, POINTER_ACC, ID, LPR, FuncRParams, RPR}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto obj_expr = expPtr(static_cast<expr*>(children[0]->ptr.release()));
        std::string method_name = children[2]->str_val;
        auto call = static_cast<fun_call*>(children[4]->ptr.release());
        
        // Create a method call expression
        auto method_call = new pointer_acc(
            children[0]->location,  // source location
            std::move(obj_expr),    // object expression
            method_name
        );
        for(size_t i = 0; i < call->args.size(); i++) {
            method_call->args.push_back(std::move(call->args[call->args.size() - i - 1]));
        }
        method_call->isFunc = true;
        res->set_node(nodePtr(method_call));
        return res;
    }),

    ruleAction(rule(PrimaryExp, {ID}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        expr *id = new identifier(res->location, children[0]->str_val);
        res->set_node(nodePtr(id));
        return res;
    }),

    // subscript
    ruleAction(rule(PrimaryExp, {PrimaryExp, LBK, Exp, RBK}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        expr *list = dynamic_cast<expr*>(children[0]->ptr.release());
        auto sub = new subscript_expr(children[0]->location, expPtr(list), expPtr(dynamic_cast<expr*>(children[2]->ptr.release())));
        res->set_node(nodePtr(sub));
        return res;
    }),
    
    // member access
    ruleAction(rule(Exp, {Exp, DOT, ID}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        expr *exp = dynamic_cast<expr*>(children[0]->ptr.release());
        auto ma = new member_access(children[0]->location, expPtr(exp), children[2]->str_val);
        res->set_node(nodePtr(ma));
        return res;
    }),
    ruleAction(rule(Exp, {Exp, DOT, ID, LPR, FuncRParams, RPR}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto obj_expr = expPtr(static_cast<expr*>(children[0]->ptr.release()));
        std::string method_name = children[2]->str_val;
        auto call = static_cast<fun_call*>(children[4]->ptr.release());
        
        // Create a method call expression
        auto method_call = new member_access(
            children[0]->location,  // source location
            std::move(obj_expr),    // object expression
            method_name
        );
        for(size_t i = 0; i < call->args.size(); i++) {
            method_call->args.push_back(std::move(call->args[call->args.size() - i - 1]));
        }
        method_call->isFunc = true;
        res->set_node(nodePtr(method_call));
        return res;
    }),

    ruleAction(rule(UnaryExp, {ID, LPR, FuncRParams, RPR}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto tail = static_cast<fun_call*>(children[2]->ptr.release());
        tail->setName(children[0]->str_val);
        tail->setLoc(children[0]->location);
        res->set_node(nodePtr(tail));
        return res;
    }),
    ruleAction(rule(UnaryExp, {ID, LPR, RPR}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto tail = new fun_call(children[0]->location);
        tail->setName(children[0]->str_val);
        tail->setLoc(children[0]->location);
        res->set_node(nodePtr(tail));
        return res;
    }),
    ruleAction(rule(UnaryExp, {UnaryOp, UnaryExp}), unaryFactory()),

    rule(UnaryOp, {PLUS}),
    rule(UnaryOp, {MINUS}),
    rule(UnaryOp, {NOT}),
    rule(UnaryOp, {STAR}), //pointer access

    ruleAction(rule(FuncRParams, {Exp, FuncRParamsTail}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto tail = static_cast<fun_call*>(children[1]->ptr.release());
        tail->addParam(expPtr(static_cast<expr*>(children[0]->ptr.release())));
        tail->setLoc(children[0]->location);
        res->set_node(nodePtr(tail));
        return res;
    }), 
    ruleAction(rule(FuncRParamsTail, {COMMA, Exp, FuncRParamsTail}), [](std::vector<parseInfoPtr>& children) {
        
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto tail = static_cast<fun_call*>(children[2]->ptr.release());
        tail->addParam(expPtr(static_cast<expr*>(children[1]->ptr.release())));
        res->set_node(nodePtr(tail));
        return res;
    }),
    ruleAction(rule(FuncRParamsTail, {}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(std::pair<size_t, size_t>{-1, -1}); //loc not used
        fun_call *p = new fun_call(std::pair<size_t, size_t>{-1, -1});
        res->set_node(nodePtr(p));
        return res;
    }),

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
    ruleAction(rule(RelExp, {RelExp, RelOp, AddExp}), binaryFactory()),
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
                    bool resolved = false;

                    if(!resolved) {
                        printLR1Items(curState.items);
                        std::cout << "conflict(1)! entry already exist! \n";
                        exit(0);
                    }
   
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
                // printLR1Items(curState.items);
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

                    const auto &curTok = input[i];

                    parseInfoPtr ptr = std::make_unique<parseInfo>(curTok.location);

                    ptr->set_str(curTok.value);
                    ptr->set_kind(curSym);
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
                                    .node = nullptr
                                };
                    }

                    // Pop right-hand side
                    auto parent = std::make_unique<parseTreeNode>(r.left);
                    std::vector<parseInfoPtr> RHSChildren;
                    for (size_t j = 0; j < r.right.size(); j++) {
                        symStack.pop_back();
                        stateStack.pop_back();
                        RHSChildren.push_back(std::move(infoStack.back()));
                        infoStack.pop_back();
                    }

                    std::reverse(RHSChildren.begin(), RHSChildren.end());
                    

                    infoStack.push_back(
                        ruleWithAction[a.n].action(RHSChildren)
                        // nullptr
                    );
                    
                    // std::cout << "infoStack size:" << infoStack.size() << std::endl;
                    auto ptr = infoStack.back().get();
                    if(ptr != nullptr && debug) {
                        if(ptr->ptr != nullptr) {
                            ptr->ptr->printAST("", "");
                        }
                    }

                    std::reverse(parent->children.begin(), parent->children.end());

                    // Push left-hand side
                    symStack.push_back(r.left);

                    // Goto new state
                    curState = stateStack.back();
                    if (table[curState].count(r.left) == 0) {
                        std::cerr << "PARSE ERROR: No GOTO for state " << curState 
                                  << " and nonterminal '" << symbolTypeNames.at(r.left) << "'\n";
                        return parseResult{
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
                                    .node = std::move(infoStack.back())
                                };
                    } else {
                        if (debug) {
                            std::cout << "opps, there are some unread text:" << symbolTypeNames.at(curSym) << " \n";
                        }
                        return parseResult{
                                    .node = nullptr
                                };
                    }
                }
                case GOTO: {
                    std::cerr << "PARSE ERROR: Unexpected GOTO action\n";
                    return parseResult{
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
        std::cout << "Failed to open CSV file\n";
        exit(1);
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
            std::cout << "Unknown symbol in CSV: " + cell +"\n";
            exit(1);
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