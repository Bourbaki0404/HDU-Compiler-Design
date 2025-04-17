#ifndef __PARSER_H
#define __PARSER_H

#include "common/common.hpp"
#include "lexer/token.hpp"
#include "lexer/lexer.hpp"
#include "parser/astnodes/node.hpp"

enum symbolType {
    START,

    // Identifiers and literals
    ID, NUM, STRING,

    // Brackets and delimiters
    LPR, RPR,        // ( )
    LBK, RBK,        // [ ]
    LBC, RBC,        // { }
    COMMA, SEMICOLON, COLON, DOT,

    // Operators
    ASSIGN,         // =
    PLUS, MINUS,    // + -
    STAR, SLASH,    // * /
    PERCENT,        // %
    PLUS_PLUS, MINUS_MINUS, // ++ --
    PLUS_EQ, MINUS_EQ,      // += -=
    STAR_EQ, SLASH_EQ,      // *= /=
    PERCENT_EQ,             // %=
    EQ, NEQ,                // == !=
    LT, GT,                 // < >
    LTE, GTE,               // <= >=
    AND, OR,                // && ||
    NOT,                    // !
    BIT_AND, BIT_OR, BIT_XOR, // & | ^
    SHL, SHR,               // << >>
    
    // Keywords
    KW_IF, KW_ELSE, KW_WHILE, KW_FOR,
    KW_RETURN, KW_FUNCTION, KW_CLASS, KW_BREAK, KW_CONTINUE, KW_CONST, 

    // Type
    KW_INT, KW_VOID, KW_FLOAT, KW_STRUCT, KW_CHAR, 

    // Special
    END_OF_FILE, INVALID, 
    
    // Empty symbol
    EMPTY,

    // Nonterminals
    CompUnit, Decl, ConstDecl, ConstDeclTail, Type, ConstDef, 
    ConstDefTail, ConstInitVal, ConstInitValTail, FuncFParamTailTail,
    ConstInitValTailTail, VarDecl, VarDeclTail, VarDef, VarDefGroup, InitVal, InitValTail, 
    InitValTailTail, FuncDef, FuncFParams, FuncFParamsTail, FuncFParam, FuncFParamTail, 
    Block, BlockItem, BlockItemTail, Stmt, ReturnExp, Exp,
    LVal, LValTail, PrimaryExp, Number, UnaryExp, UnaryOp, FuncRParams, FuncRParamsTail,
    MulExp, MulOp, AddExp, AddOp, RelExp, RelOp, EqExp, EqOp, LAndExp, LOrExp, ConstExp,

    // OOP
    ClassDef, ClassBody, ClassMemberList, ClassMember, ConstructorDef
};


symbolType tokenToSymbol(token::tokenType tokType);
extern const std::unordered_map<symbolType, std::string> symbolTypeNames;

// The information carried by a specific symbol
struct parseInfo {
    public:
        parseInfo(parseInfo& p)
        : str_val(p.str_val), type(p.type), location(p.location), ptr(std::move(p.ptr)) {}

        parseInfo(std::pair<size_t, size_t> pair)
        : location(pair) {}

        void set_str(std::string str_val) {
            this->str_val = str_val;
        }

        void set_type(symbolType type) {
            this->type = type;
        }
        
        void set_node(nodePtr ptr) {
            this->ptr = std::move(ptr);
        }
        
    public:
        std::string str_val;
        // int int_val;
        symbolType type;
        std::pair<size_t, size_t> location;
        // std::vector<expPtr> dim; 
        nodePtr ptr = nodePtr(nullptr);
};

struct rule {
    public:
        rule(symbolType left, std::vector<symbolType> right)
        : left(left), right(right) {}
        bool operator==(const rule& other) const {
            if(right.size() != other.right.size() || left != other.left) return false;
            for(size_t i = 0; i < right.size(); i++) {
                if(right[i] != other.right[i]) return false;
            }
            return true;
        }
        bool operator<(const rule& other) const {
            if(left < other.left) return true;
            if(left > other.left) return false;
            for(size_t i = 0; i < std::min(right.size(), other.right.size()); i++) {
                if(right[i] < other.right[i]) return true;
                if(right[i] > other.right[i]) return false;
            }
            return right.size() < other.right.size();
        }
    public:
        symbolType left;
        std::vector<symbolType> right;
};



struct lrItem {
    public:
        lrItem(struct rule rule, size_t pos, symbolType lookahead)
        : rule(rule), pos(pos), lookahead(lookahead) {
        }
        bool operator==(const lrItem& other) const {
            return rule == other.rule &&
                lookahead == other.lookahead && pos == other.pos;
        }
        bool operator<(const lrItem& other) const {
            if(rule < other.rule) {
                return true;
            } else if(!(rule == other.rule)) {
                return false;
            } else {
                if(pos < other.pos) {
                    return true;
                } else if(pos > other.pos) {
                    return false;
                } else {
                    if(lookahead < other.lookahead) return true;
                    else return false;
                }
            }
        }
    public:
        struct rule rule;
        size_t pos;
        symbolType lookahead;
};



struct lrState {
    public:
        lrState(const std::set<lrItem> &items)
        :items(items) {}
        bool operator==(const lrState& other) const {
            if(items.size() != other.items.size()) return false;
            for(const auto &item : other.items) {
                if(items.count(item) == 0)
                    return false;
            }
            return true;
        }
    public:
        std::set<lrItem> items;
};

enum actionType {
    SHIFT,
    REDUCE,
    GOTO,
    ACC
};

struct action {
    public:
        action(actionType type, size_t n)
        : type(type), n(n) {}
        action(const action &a)
        : type(a.type), n(a.n) {}
        action() {}
    public:
        actionType type;
        size_t n;
};

struct parseTreeNode {
    symbolType type;
    std::vector<std::unique_ptr<parseTreeNode>> children;
    parseTreeNode(symbolType sym) 
        : type(sym) {}

    void addChild(std::unique_ptr<parseTreeNode> child) {
        children.push_back(std::move(child));
    }
};
using parserTreePtr = std::unique_ptr<parseTreeNode>;
using firstFollowSet = std::unordered_map<symbolType, std::set<symbolType>>;
using parseInfoPtr = std::unique_ptr<parseInfo>;
using SemanticAction = std::function<parseInfoPtr(std::vector<parseInfoPtr>&)>;

struct ruleAction {
    public:
        ruleAction(struct rule rule, SemanticAction action)
        : rule(rule), action(action) {}
        ruleAction(struct rule rule)
        : rule(rule), action([rule](std::vector<parseInfoPtr> &children){
            if(children.size() > 0) {
                return std::move(children[0]);
            } else if(children.empty()) {
                return parseInfoPtr(nullptr);
            }
        }){}
    public:
        struct rule rule;
        SemanticAction action;
};

struct parseResult {
    parserTreePtr parseTree;
    nodePtr node;
};

void visualizeAsTree(const parserTreePtr &node, const std::string& prefix, bool isLast);


void printAllRules(const std::vector<rule>& rules);
std::set<symbolType> computeNonterminals(const std::vector<rule> &rules);
void printNonterminals(const std::set<symbolType>& nonterminals);
firstFollowSet computeFirstSet(const std::vector<rule> &rules);
void printFirstOrFollowSets(const firstFollowSet &Sets, bool first);
firstFollowSet computeFollowSet(const std::vector<rule> &rules, const firstFollowSet &firstSet); 
std::set<lrItem> computeClosure(const std::set<lrItem> &coreItems, const firstFollowSet &firstSet);
void printLR1Items(const std::set<lrItem>& items);
lrState computeNextState(lrState state, symbolType edge, const firstFollowSet &firstSet); 
using LRTable = std::vector<std::unordered_map<symbolType, action>>;
LRTable computeLRTable(const firstFollowSet &firstSet, lrState start);
void exportLRTableToCSV(const LRTable& table, const std::string& filename);
extern const std::vector<rule> parserRules;
parseResult Parse(std::vector<token> input, const LRTable &table, bool debug = false);
LRTable importLRTableFromCSV(const std::string& filename);



#endif