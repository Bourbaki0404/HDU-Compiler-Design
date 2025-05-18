#include "Transform/Mem2Reg.hpp"
#include "IR/Function.hpp"
#include "IR/basicBlock.hpp"
#include "IR/instruction.hpp"
#include "IR/Cast.hpp"
#include "IR/utils.hpp"
#include "Analysis/DominatorTree.hpp"
#include "Pass/pass.h"
#include "IR/IRBuilder.hpp"
#include "IR/CFG.hpp"
#include "IR/Value.hpp"


namespace IR{



// Variables with no aliases are trivial register promotion candidates.
// A scalar variable can have aliases whenever its address is taken, or if it is a global variable, since it can be accessed by function calls.
// We employ naive strategy to check if the alloca is used by a instruction other than load or store.
// We assume that all memory variables are nonvolatile.
bool Mem2Reg::isAllocaPromotable(AllocaInst *AI) {
    for (const Instruction *I : AI->uses()) {
        if (LoadInst *LI = dyn_cast<LoadInst>(I)) {

            /// load is use
            allocaInfoMap[AI].UseBlocks.insert(LI->getParent());
        } else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
            if (SI->getValueOperand() == AI ||
                SI->getValueOperand()->getType() != AI->getAllocatedType()) {
                return false;
            } 

            /// store is def
            allocaInfoMap[AI].DefBlocks.insert(SI->getParent());
        }
    }
    return true;
}

void Mem2Reg::rename(Function &F) {
    rename(&(F.getEntryBlock()));
}

void Mem2Reg::rename(BasicBlock *BB) {
    visitedDFSBlocks.insert(BB);
    for(auto &inst : *BB) {
        if(StoreInst *SI = dyn_cast<StoreInst>(&inst)) {
            /// store is def
            AllocaInst *AI = dyn_cast<AllocaInst>(SI->getPointerOperand());
            __assert__(AI, "AI is nullptr");
            allocaStackMap[AI].push(SI);
        } else if(LoadInst *LI = dyn_cast<LoadInst>(&inst)) {
            /// load is use
            AllocaInst *AI = dyn_cast<AllocaInst>(LI->getOperand(0));
            __assert__(AI, "AI is nullptr");
            __assert__(allocaStackMap[AI].size() > 0, "no def for the alloca");
            LI->replaceAllUsesWith(allocaStackMap[AI].top());
        } else if(PHINode *PHI = dyn_cast<PHINode>(&inst)) {
            AllocaInst *AI = phiToAlloca[PHI];
            __assert__(AI, "AI is nullptr");
            /// phi is a def
            allocaStackMap[AI].push(PHI);
        }
    }

    /// add the incoming values to the phi nodes
    for(auto bb : successors(BB)) {
        /// iterate over the phi nodes in the block
        for(auto &inst : *bb) {
            if(PHINode *PHI = dyn_cast<PHINode>(&inst)) {
                AllocaInst *AI = phiToAlloca[PHI];
                __assert__(AI, "AI is nullptr");
                __assert__(allocaStackMap[AI].size() > 0, "no def for the alloca");
                PHI->addIncoming(allocaStackMap[AI].top(), BB);
            } else {
                /// phi nodes are at the top of the stack
                break;
            }
        }
    }

    /// rename the children in the dominator tree
    for(BasicBlock *Child : DT->getChildren(BB)) {
        if(visitedDFSBlocks.find(Child) == visitedDFSBlocks.end()) {
            rename(Child);
        }
    }

    for(auto &inst : *BB) {
        if(StoreInst *SI = dyn_cast<StoreInst>(&inst)) {
            AllocaInst *AI = dyn_cast<AllocaInst>(SI->getPointerOperand());
            __assert__(AI, "AI is nullptr");
            __assert__(allocaStackMap[AI].size() > 0, "no def for the alloca");
            allocaStackMap[AI].pop();
        } else if(PHINode *PHI = dyn_cast<PHINode>(&inst)) {
            AllocaInst *AI = phiToAlloca[PHI];
            __assert__(AI, "AI is nullptr");
            __assert__(allocaStackMap[AI].size() > 0, "no def for the alloca");
            allocaStackMap[AI].pop();
        }
    }
}

void Mem2Reg::PromoteMemToReg(std::vector<AllocaInst *> &Allocas) {
    for (AllocaInst *AI : Allocas) {
        std::cout << "PromoteMemToReg" << std::endl;
        /// working list contains all the unprocessed basic blocks that has defs of the memory variable
        std::unordered_set<BasicBlock *> workingList;

        /// inserted contains all the basic blocks that has been inserted with phi nodes
        std::unordered_set<BasicBlock *> inserted;

        workingList.insert(allocaInfoMap[AI].DefBlocks.begin(), allocaInfoMap[AI].DefBlocks.end());

        while(!workingList.empty()) {
            BasicBlock *BB = *workingList.begin();
            workingList.erase(BB);


            for(BasicBlock *DF : dominanceFrontiers[BB]) {
                if(inserted.find(DF) == inserted.end()) {
                    /// Insert PHI nodes at the beginning of the block
                    insertPHINodes(AI, DF);
                    inserted.insert(DF);

                    /// if new defs are added, add them to the working list
                    if(allocaInfoMap[AI].DefBlocks.find(DF) == allocaInfoMap[AI].DefBlocks.end()) {
                        workingList.insert(DF);
                    }
                }
            }
        }
    }
}

bool Mem2Reg::promoteMemoryToRegister(Function &F) {
    __assert__(DT, "Dominator tree is not set");
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
        rename(F);
        // std::cout << "Promoted " << Allocas.size() << " allocas" << std::endl;
        Changed = true;
        break;
    }
    return Changed;
}

void Mem2Reg::insertPHINodes(AllocaInst *AI, BasicBlock *DF) {
    /// insert phi nodes at the beginning of the block
    IRBuilder builder;
    builder.setInsertPoint(&(*DF->begin()));

    /// the number of reserved operands is the number of predecessors of the block
    unsigned numReserved = 0;
    for(BasicBlock *Pred : predecessors(DF)) { /// since no fast method to get the size of a ilist.
        numReserved++;
    }
    PHINode *PHI = builder.CreatePHI(AI->getAllocatedType(), numReserved);

    phiToAlloca[PHI] = AI;
}


void Mem2Reg::computeDominanceFrontier(Function &F) {
    computeDominanceFrontier(&F.getEntryBlock());
}

void Mem2Reg::computeDominanceFrontier(BasicBlock *BB) {
    /// Recursively compute the dominance frontier of the block
    /// DT_LOCAL
    visitedBlocks.insert(BB);
    for(BasicBlock *Succ : successors(BB)) {
        if(DT->getIDom(Succ) != BB) {
            dominanceFrontiers[BB].insert(Succ);
        }
    }

    /// DT_UP
    for(size_t i = 0; i < DT->getNumChildren(BB); i++) {
        BasicBlock *Child = DT->getChild(BB, i);
        if(visitedBlocks.find(Child) == visitedBlocks.end()) {
            computeDominanceFrontier(Child);
            visitedBlocks.insert(Child);
        }
        for(BasicBlock *DF : dominanceFrontiers[Child]) {
            if(!DT->dominates(BB, DF)) {
                dominanceFrontiers[BB].insert(DF);
            }
        }
    }
}


bool Mem2Reg::runOnFunction(Function &F) {

    // We need the dominator tree for this pass
    DT = getAnalysisResult<DominatorTreeWrapper>(F);

    // Compute the dominance frontier
    computeDominanceFrontier(F);

    return promoteMemoryToRegister(F);
}

}