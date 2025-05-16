#include "Analysis/DominatorTree.h"
#include "IR/CFG.hpp"

namespace IR {

bool DominatorTreeResult::dominates(const BasicBlock *A, const BasicBlock *B) const {

}

bool DominatorTree::runOnFunction(Function &F) {
    // Clear any previous results
    DTResult.Doms.clear();
    DTResult.DomFrontiers.clear();
    
    // First compute dominators
    calculateDoms(F);
    
    // Then compute dominance frontiers
    calculateDomFrontiers(F);
    
    return false; // Dominance calculation doesn't modify the function
}



// An algorithm for finding dominators:

// dom = {every block -> all blocks}
// dom[entry] = {entry}
// while dom is still changing:
//     for vertex in CFG except entry:
//         dom[vertex] = {vertex} ∪ ⋂(dom[p] for p in vertex.preds}

void DominatorTree::calculateDoms(Function &F) {
    
}

void DominatorTree::calculateDomFrontiers(Function &F) {
 
}

} // end namespace IR 