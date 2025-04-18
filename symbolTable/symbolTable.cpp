#include "symbolTable/symbolTable.hpp"


SymbolTable::SymbolTable() {
    scopes.push_back({});//the global env
}
bool SymbolTable::insert(const std::string &symbol, Symbol sym)
{
    auto& table = scopes.back();
    if (table.find(symbol) != table.end()) {
        return false; 
    }
    table[symbol] = sym;
    return true;
}

bool SymbolTable::exists(const std::string &symbol)
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(symbol) != it->end()) {
            return true; 
        }
    }
    return false; 
}

bool SymbolTable::isInCurrentScope(const std::string& symbol) {
    if(scopes.back().find(symbol) == scopes.back().end()) return false;
    return true;
}

Symbol SymbolTable::getValue(const std::string &symbol)
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(symbol) != it->end()) {
            return (*it)[symbol]; // Return the symbol's value
        }
    }
    return Symbol{Unknown, nullptr, nullptr};
}

void SymbolTable::beginScope()
{
    depth++;
    // cout << "enter new scope" << endl;
    scopes.push_back({});
}

void SymbolTable::endScope()
{
    if(depth > 0)
        depth--;
    // cout << "quit scope" << endl;
    if (!scopes.empty()) {
        scopes.pop_back(); // Remove the current scope
    }
}

void SymbolTable::printCurScope()
{
    for(const auto &sym : scopes.back()) {
        std::cout << "current scope: " << sym.first + " " << sym.second.type->to_string() << " depth:" << depth << std::endl;
    }
}
