#ifndef __SYMBOLTABLE_H
#define __SYMBOLTABLE_H

#include "common/common.hpp"
#include "types/types.hpp"

struct SymbolTable;
using SymTblPtr = std::unique_ptr<SymbolTable>;

enum symbolKind {
    VARIABLE, FUNCTION, CLASS_DEF, Unknown
};

struct Type;

// Symbol entry
struct Symbol {
    symbolKind kind;
    struct Type *type;
    void *data;//index into the constant table
};

class SymbolTable {
public:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    int depth = 0;
public:
    SymbolTable();
    bool insert(const std::string& symbol, Symbol sym);
    bool exists(const std::string& symbol);
    Symbol getValue(const std::string& symbol);
    void beginScope();
    void endScope();
    void printCurScope();
};

#endif