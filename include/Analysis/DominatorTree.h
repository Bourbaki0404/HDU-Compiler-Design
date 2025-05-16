#pragma once

#include "Pass/pass.h"
#include "IR/Function.hpp"
#include "IR/basicBlock.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

namespace IR {

/// DominatorTree - Concrete subclass of DominatorTreeBase that is used to
/// compute a normal dominator tree.
///
class DominatorTreeResult : public AnalysisResult {
    friend class DominatorTree;
public:
    using DomTreeNodeMap = std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>>;
    using DomFrontierMap = std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>>;

    /// Calculate and return the dominance frontiers for each basic block
    const DomFrontierMap &getDomFrontiers() const { return DomFrontiers; }

    
private:
    /// Map each basic block to its dominator
    DomTreeNodeMap Doms;
    
    /// Map each basic block to its dominance frontier
    DomFrontierMap DomFrontiers;
};

/// DominatorTree Analysis Pass - Compute a dominator tree for a function
class DominatorTree : public AnalysisPass<DominatorTreeResult>, public FunctionPass {
public:
    /// Run the dominator tree algorithm on the function
    bool runOnFunction(Function &F) override;
    
    /// Get the analysis result
    DominatorTreeResult *getResult() override { return &DTResult; }
    
    /// Recalculate the dominator tree for a given function
    void recalculate(Function &F) {
        runOnFunction(F);
    }
    
private:
    /// Compute the dominator for each node in the CFG using dataflow analysis
    void calculateDoms(Function &F);
    
    /// Compute the dominance frontier for each node
    void calculateDomFrontiers(Function &F);
    
    DominatorTreeResult DTResult;
};

} // end namespace IR 