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

    // insert a symbol into the current scope
    bool insert(const std::string& symbol, Symbol sym);

    // return true if the symbol exists in any scope
    bool exists(const std::string& symbol);

    // return true if the symbol exists at current scope 
    bool isInCurrentScope(const std::string& symbol);

    // get the item of the table
    Symbol getValue(const std::string& symbol);
    void beginScope();
    void endScope();
    void printCurScope();
};

#endif