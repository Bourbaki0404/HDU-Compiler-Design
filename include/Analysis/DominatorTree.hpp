#pragma once

#include "Pass/pass.h"
#include "IR/Function.hpp"
#include "IR/basicBlock.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "common/Graph.hpp"
#include <queue>

namespace IR {


/// DominatorTree - Concrete subclass of DominatorTreeBase that is used to
/// compute a normal dominator tree.
class DominatorTreeResult : public AnalysisResult {
    friend class DominatorTreeWrapper;
public:
    DominatorTreeResult() = default;

    /// Get the immediate dominator of a basic block
    BasicBlock *getIDom(BasicBlock *bb) const { return idomMap.at(bb); }

    /// Get the children of a basic block, less efficient call
    std::unordered_set<BasicBlock *> getChildren(BasicBlock *bb) const { return childrenMap.at(bb); }

    /// Get the number of children of a basic block
    size_t getNumChildren(BasicBlock *bb) const { return childrenMap.at(bb).size(); }

    /// Get the child of a basic block at index
    BasicBlock *getChild(BasicBlock *bb, size_t index) const { return *std::next(childrenMap.at(bb).begin(), index); }  

    /// Dump the dominator tree
    void dump() const;

private:
    /// reference to the idom map in the dominator tree wrapper
    std::unordered_map<BasicBlock *, BasicBlock *> idomMap;
    
    /// reference to the children map in the dominator tree wrapper
    std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> childrenMap;
};

/// DominatorTree Analysis Pass - Compute a dominator tree for a function
/// Using the Lengauer-Tarjan algorithm
class DominatorTreeWrapper : public AnalysisPass<DominatorTreeResult>, public FunctionPass {
public:

    DominatorTreeWrapper()
    : FunctionPass() {}

    /// Run the dominator tree algorithm on the function
    bool runOnFunction(Function &F) override;
    
    /// Get the analysis result
    DominatorTreeResult *getAnalysisResult() override { return &DTResult; }

    struct nodeInfo {
        /// the dfn of the node
        size_t dfn;

        /// the parent of the node
        BasicBlock *parent = nullptr;

        /// the ancestor of the node
        BasicBlock *ancestor = nullptr;

        /// the semi-dominator of the node
        BasicBlock *sdom = nullptr;

        /// the bucket of the node
        std::unordered_set<BasicBlock *> bucket;
    };
    
private:
    /// Compute the dominator tree using the Lengauer-Tarjan algorithm
    void computeIDom(Function &F);

    /// Evaluate the semi-dominator of a node
    BasicBlock *eval(BasicBlock *u);

    /// Set the ancestor of a node 
    void link(BasicBlock *u, BasicBlock *v);

    /// The map of the node info
    std::unordered_map<BasicBlock *, nodeInfo> nodeMap;

    /// Map from dfn to basic block
    std::vector<BasicBlock *> dfnVector;

    /// The result of the dominator tree analysis
    DominatorTreeResult DTResult;
};

} // end namespace IR 