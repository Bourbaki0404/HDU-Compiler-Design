#pragma once

#include "common/common.hpp"
#include "Pass/pass.h"
#include "Analysis/DominatorTree.hpp"
#include "IR/Function.hpp"
#include "IR/instruction.hpp"

namespace IR {

/**
 * LLVM assumes that all locals are introduced in the entry basic block of a function with an alloca instruction. LLVM also assumes that all allocas appear at the start of the entry block continuously. This assumption could be easily violated by the front-end, but it is a reasonable assumption to make.
 *
 * Variables with no aliases are trivial register promotion candidates. They include the temporaries generated during PRE to hold the values of redundant computations. 
 * Variables in the program can also be determined via compiler analysis or
 * by language rules to be alias-free. For those trivial candidates, one can rename
 * them to unique pseudo-registers, and no load or store needs to be generated.
 * 
 * A scalar variable can have aliases whenever its address is taken, or if it is a global variable, since it can be accessed by function calls. 
 *
 * For each local selected for promotion in step 2, a list of blocks which use it, and a list of block which define it are maintained by making a linear sweep over each instruction of each block.
 * Some basic optimizations are performed:
 * 1) Unused allocas are removed.
 * 2) If there is only one defining block for an alloca, all loads which are dominated by the definition are replaced with the value.
 * 3) allocas which are read and written only in a block can avoid traversing CFG, and PHI-node insertion by simply inserting each load with the value from nearest store.
 *
 * A dominator tree is constructed.
 *
 * For each candidate for promotion, points to insert PHI nodes is computed as follows:
 * 1) A list of blocks which use it without defining it (live-in blocks or upward exposed blocks) are determined with the help of using and defining blocks created in Step 3.
 * 2) A priority queue keyed on dominator tree level is maintained so that inserted nodes corresponding to defining blocks are handled from the bottom of the dominator tree upwards. This is done by giving each block a level based on its position in the dominator tree.
 *
 * For each node — root, in the priority queue:
 * 1) Iterated dominance frontier of a definition is computed by walking all dominator tree children of root, inspecting their CFG edges with targets elsewhere on the dominator tree. Only targets whose level is at most root level are added to the iterated dominance frontier.
 * 2) PHI-nodes are inserted at the beginning in each block in the iterated dominance frontier computed in the previous step. There will be predecessor number of dummy argument to the PHI function at this point.
 *
 * Once all PHI-nodes are prepared, a rename phase start with a worklist containing just entry block as follows:
 * 1) A hash table of IncomingVals which is a map from a alloca to its most recent name is created. Most recent name of each alloca is an undef value to start with.
 *
 * While (worklist != NULL)
 *
 * Remove block B from worklist and mark B as visited.
 *
 * For each instruction in B:
 * 1) If instruction is a load instruction from location L (where L is a promotable candidate) to value V, delete load instruction, replace all uses of V with most recent value of L i.e, IncomingVals[L].
 * 2) If instruction is a store instruction to location L (where L is a promotable candidate) with value V, delete store instruction, set most recent name of L i.e, IncomingVals[L] = V.
 * 3) If instruction is a PHI-node, fill the corresponding PHI-node argument with most recent name for that location i.e, IncomingVals[L].
 * 4) For each PHI-node corresponding to a alloca — L , in each successor of B, fill the corresponding PHI-node argument with most recent name for that location i.e, IncomingVals[L].

 * Add each unvisited successor of B to worklist.
 */

/// Mem2Reg is a pass that promotes memory to register
/// It is a wrapper of the promoteMemoryToRegister function
class Mem2Reg : public FunctionPass {
public:
    bool runOnFunction(Function &F) override;

private:
    bool promoteMemoryToRegister(Function &F);
    void PromoteMemToReg(std::vector<AllocaInst *> &Allocas);

    // SSA Construction
    void insertPHINodes(AllocaInst *AI);

private:
    struct allocaInfo {
        std::vector<BasicBlock *> DefBlocks;
        std::vector<BasicBlock *> UseBlocks;
    };  
    
    using AllocaInfoMap = std::unordered_map<AllocaInst *, allocaInfo>;
    AllocaInfoMap allocaInfoMap;
};

} // end namespace IR