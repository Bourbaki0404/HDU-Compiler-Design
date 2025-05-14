#include "include/Transform/Mem2Reg.h"
#include "include/IR/Function.hpp"
#include "include/IR/basicBlock.hpp"
#include "include/IR/instruction.hpp"
#include "include/IR/Cast.hpp"
#include "include/IR/utils.hpp"

namespace IR{

bool isAllocaPromotable(const AllocaInst *AI) {
  // Only allow direct and non-volatile loads and stores...
    for (const Use *U : AI->uses()) {
        Value *user = U->getUser();
        if (const LoadInst *LI = dyn_cast<LoadInst>(user)) {
            // Note that atomic loads can be transformed; atomic semantics do
            // not have any meaning for a local alloca.
            // if (LI->isVolatile())
            return true;
        } else if (const StoreInst *SI = dyn_cast<StoreInst>(user)) {
        if (SI->getValueOperand() == AI ||
            SI->getValueOperand()->getType() != AI->getAllocatedType())
            return false; // Don't allow a store OF the AI, only INTO the AI.
        // Note that atomic stores can be transformed; atomic semantics do
        // not have any meaning for a local alloca.
        // if (SI->isVolatile())
        //     return false;
        } else if (const IntrinsicInst *II = dyn_cast<IntrinsicInst>(U)) {
        if (!II->isLifetimeStartOrEnd() && !II->isDroppable())
            return false;
        } else if (const BitCastInst *BCI = dyn_cast<BitCastInst>(U)) {
        if (!onlyUsedByLifetimeMarkersOrDroppableInsts(BCI))
            return false;
        } else if (const GetElementPtrInst *GEPI = dyn_cast<GetElementPtrInst>(U)) {
        if (!GEPI->hasAllZeroIndices())
            return false;
        if (!onlyUsedByLifetimeMarkersOrDroppableInsts(GEPI))
            return false;
        } else if (const AddrSpaceCastInst *ASCI = dyn_cast<AddrSpaceCastInst>(U)) {
        if (!onlyUsedByLifetimeMarkers(ASCI))
            return false;
        } else {
        return false;
        }
    }

    return true;
}

static bool promoteMemoryToRegister(Function &F) {
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

        // PromoteMemToReg(Allocas);
        // std::cout << "Promoted " << Allocas.size() << " allocas" << std::endl;
        Changed = true;
    }
    return Changed;
}



bool Mem2Reg::runOnFunction(Function &F) {
    return promoteMemoryToRegister(F);
}

}