#include "Transform/Mem2Reg.h"
#include "IR/Function.hpp"
#include "IR/basicBlock.hpp"
#include "IR/instruction.hpp"
#include "IR/Cast.hpp"
#include "IR/utils.hpp"
#include "Analysis/DominatorTree.hpp"

namespace IR{



// Variables with no aliases are trivial register promotion candidates.
// A scalar variable can have aliases whenever its address is taken, or if it is a global variable, since it can be accessed by function calls.
// We employ naive strategy to check if the alloca is used by a instruction other than load or store.
// We assume that all memory variables are nonvolatile.
bool isAllocaPromotable(const AllocaInst *AI) {
    for (const Instruction *I : AI->uses()) {
        if (const LoadInst *LI = dyn_cast<LoadInst>(I)) {
            continue;
        } else if (const StoreInst *SI = dyn_cast<StoreInst>(I)) {
        if (SI->getValueOperand() == AI ||
            SI->getValueOperand()->getType() != AI->getAllocatedType())
            return false; // Don't allow a store OF the AI, only INTO the AI.
            // Note that atomic stores can be transformed; atomic semantics do
            // not have any meaning for a local alloca.
        }
    }
    return true;
}

void Mem2Reg::PromoteMemToReg(std::vector<AllocaInst *> &Allocas) {
    for (AllocaInst *AI : Allocas) {

        // Collect alloca info
        for(const Instruction *I : AI->uses()) {
            if(LoadInst *LI = dyn_cast<LoadInst>(I)) {
                allocaInfoMap[AI].DefBlocks.push_back(LI->getParent());
            } else if(StoreInst *SI = dyn_cast<StoreInst>(I)) {
                allocaInfoMap[AI].UseBlocks.push_back(SI->getParent());
            }
        }

        // Insert PHI nodes at the beginning of the block
        insertPHINodes(AI);





    }
}

bool Mem2Reg::promoteMemoryToRegister(Function &F) {
    std::vector<AllocaInst *> Allocas;
    BasicBlock &BB = F.getEntryBlock(); // Get the entry node for the function
    bool Changed = false;

    while (true) {
        Allocas.clear();

        // Find allocas that are safe to promote, by looking at all instructions in
        // the entry node
        for (BasicBlock::iterator I = BB.begin(); I != BB.end(); ++I)
        if (AllocaInst *AI = dyn_cast<AllocaInst>(&*I)) // Is it an alloca?
            if (isAllocaPromotable(AI))
            Allocas.push_back(AI);

        if (Allocas.empty())
        break;

        PromoteMemToReg(Allocas);
        // std::cout << "Promoted " << Allocas.size() << " allocas" << std::endl;
        Changed = true;
    }
    return Changed;
}

// See SSA book Algorithm 3.1: Standard algorithm for inserting φ-functions
void Mem2Reg::insertPHINodes(AllocaInst *AI) {

}


bool Mem2Reg::runOnFunction(Function &F) {
    // We need the dominator tree for this pass
    
    
    return promoteMemoryToRegister(F);
}

}