#pragma once
#include "common/common.hpp"
#include "IR/Value.hpp"

namespace IR{

struct Argument;
struct BasicBlock;
struct Function;
struct GlobalAlias;
struct GlobalIFunc;
struct GlobalVariable;
struct Instruction;

struct ValueSymbolTable {
    /// A mapping of names to values.
    using ValueMap = std::unordered_map<std::string, Value*>;

    /// An iterator over a ValueMap.
    using iterator = ValueMap::iterator;

    inline iterator begin() { return vmap.begin(); }

    /// Get an iterator to the end of the symbol table.
    inline iterator end() { return vmap.end(); }

    /// @}
    /// @name Accessors
    /// @{

    /// This method finds the value with the given \p Name in the
    /// the symbol table.
    /// @returns the value associated with the \p Name
    /// Lookup a named Value.
    Value *lookup(const std::string &Name) const {
        if(vmap.find(Name) != vmap.end()) {
            return vmap.at(Name);
        }
        return nullptr;
    }

    std::string makeUniqueName(Value *V, const std::string &oldname);

    // Try to insert a named value into the table. Value might be renamed automatically to avoid conflict.
    void reinsertValue(Value *V);

    void removeValueName(const std::string &Name) {
        vmap.erase(Name);
    }

    bool exist(const std::string &Name) const {
        return vmap.find(Name) != vmap.end();
    }

    /// @returns true iff the symbol table is empty
    /// Determine if the symbol table is empty
    inline bool empty() const { return vmap.empty(); }

    /// The number of name/type pairs is returned.
    inline unsigned size() const { return unsigned(vmap.size()); }


    /// This function can be used from the debugger to display the
    /// content of the symbol table while debugging.
    /// Print out symbol table on stderr
    void dump() {
        // for (auto &I : *this) {
        //     I.second->dump();
        // }
    }


    unsigned LastUnique = 0;



    ///< The map that holds the symbol table.
    ValueMap vmap;                    
};

}