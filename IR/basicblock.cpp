#include "IR/basicBlock.hpp"
#include "IR/valueSymbolTable.hpp"
#include "IR/Function.hpp"
#include "IR/utils.hpp"

namespace IR{


bool BasicBlock::isEntryBlock() const {
    const Function *F = getParent();
    __assert__(F, "Block must have a parent function to use this API");
    return this == &F->getEntryBlock();
}

const Module *BasicBlock::getModule() const { 
    __assert__(getParent(), "BasicBlock has no parent\n");
    return getParent()->getParent(); 
}

Module *BasicBlock::getModule() {
    __assert__(getParent(), "BasicBlock has no parent\n");
    return getParent()->getParent(); 
}

ValueSymbolTable *BasicBlock::getValueSymbolTable() {
    if(auto *F = getParent()) {
        return F->getValueSymbolTable();
    }
    return nullptr;
}

void BasicBlock::setParent(struct Function *parent) {
    this->parent = parent;
    // Set Parent=parent, updating instruction symtab entries as appropriate.
    InstList.setSymTabObject(&this->parent, parent);
}


}

