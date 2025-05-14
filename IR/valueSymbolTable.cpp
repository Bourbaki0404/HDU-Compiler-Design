#include "IR/valueSymbolTable.hpp"
#include "IR/utils.hpp"


namespace IR{

std::string ValueSymbolTable::makeUniqueName(Value *V, const std::string &oldname) {
    unsigned BaseSize = oldname.size();
    std::string uniqueName = oldname;
    while (true) {
        uniqueName = oldname + std::to_string(++LastUnique);

        // Try insert the vmap entry with this suffix.
        auto IterBool = vmap.insert(std::make_pair(uniqueName, V));
        if (IterBool.second) return uniqueName;
    }
}


void ValueSymbolTable::reinsertValue(Value *V) {
    assert(V->hasName() && "Can't insert nameless Value into symbol table");

    // Try inserting the name, assuming it won't conflict.
    if (vmap.insert({V->getName(), V}).second) {
        return;
    }
    const auto &uniqueName = makeUniqueName(V, V->getName());

    // simply set the name of the value, do not modify the symbol table
    V->__setName__(uniqueName);
}




}