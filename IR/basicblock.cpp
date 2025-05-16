#include "IR/basicBlock.hpp"
#include "IR/valueSymbolTable.hpp"
#include "IR/Function.hpp"
#include "IR/utils.hpp"
#include "IR/instruction.hpp"

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

void BasicBlock::removeFromParent() {
    if(auto *F = getParent()) {
        F->getBasicBlockList().remove(getIterator());
    }
}

// void BasicBlock::replacePhiUsesWith(BasicBlock *Old, BasicBlock *New) {
//     // N.B. This might not be a complete BasicBlock, so don't assume
//     // that it ends with a non-phi instruction.
//     for (Instruction &I : *this) {
//         PHINode *PN = dyn_cast<PHINode>(&I);
//         if (!PN)
//         break;
//         PN->replaceIncomingBlockWith(Old, New);
//     }
// }


// void BasicBlock::replaceSuccessorsPhiUsesWith(BasicBlock *Old,
//                                               BasicBlock *New) {
//     Instruction *TI = getTerminator();
//     if (!TI)
//         return;
//     for (BasicBlock *Succ : successors(TI))
//         Succ->replacePhiUsesWith(Old, New);
// }

void BasicBlock::setParent(struct Function *parent) {
    this->parent = parent;
    // Set Parent=parent, updating instruction symtab entries as appropriate.
    InstList.setSymTabObject(&this->parent, parent);
}

Instruction *BasicBlock::getTerminator() {
    if(InstList.empty() || InstList.back().isTerminator()) {
        return nullptr;
    } else {
        return &InstList.back();
    }
}

const Instruction *BasicBlock::getTerminator() const {
    if(InstList.empty() || InstList.back().isTerminator()) {
        return nullptr;
    } else {
        return &InstList.back();
    }
}


}

