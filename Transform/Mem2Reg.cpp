#include "include/Transform/Mem2Reg.h"
#include "include/IR/Function.hpp"
#include "include/IR/basicBlock.hpp"
#include "include/IR/instruction.hpp"
#include "include/IR/Cast.hpp"
#include "include/IR/utils.hpp"

namespace IR{

static bool isAllocaPromotable(AllocaInst *AI) {
    return 
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