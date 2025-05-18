#include "Analysis/DominatorTree.hpp"
#include "IR/CFG.hpp"
#include "common/Graph.hpp"

namespace IR {

void DominatorTreeResult::dump() const {
    std::cout << "Dominator Tree Analysis Result:" << std::endl;
    for(auto &bb : idomMap) {
        std::cout << "Basic Block: " << bb.first->getName() << std::endl;
        bb.first->print();
        if(bb.second != nullptr) {
            std::cout << "Immediate Dominator: " << bb.second->getName() << std::endl;
            bb.second->print();
        } else {
            std::cout << "entry block has no immediate dominator" << std::endl;
        }
    }
}

bool DominatorTreeWrapper::runOnFunction(Function &F) {
    computeIDom(F);
    return true;
}

/// find the ancestor of u (with greater dfn than u by searching the ancestor chain) with the smallest sdom
BasicBlock *DominatorTreeWrapper::eval(BasicBlock *u) {
    BasicBlock *v = u;
    while(nodeMap[v].ancestor != nullptr) {
        if(nodeMap[v].sdom < nodeMap[u].sdom) {
            u = v;
        }
        v = nodeMap[v].ancestor;
    }
    return u;
}

/// Set the ancestor of a node 
void DominatorTreeWrapper::link(BasicBlock *u, BasicBlock *v) {
    nodeMap[u].ancestor = v;
}

/// Compute the dominator tree using the Lengauer-Tarjan algorithm
/// 1. Compute the semi-dominators and immediate dominators of the basic blocks
/// 2. Build the dominator tree 
void DominatorTreeWrapper::computeIDom(Function &F) { 
    for(auto &bb : F) {
        nodeMap[&bb].parent = nullptr;
        nodeMap[&bb].sdom = nullptr;
        nodeMap[&bb].bucket.clear();
        nodeMap[&bb].ancestor = nullptr;
        DTResult.idomMap[&bb] = nullptr;
    }

    /// DFS on the CFG
    Graph<BasicBlock> cfg;
    cfg.setHandler([&](BasicBlock *bb) {
        nodeMap[bb].dfn = dfnVector.size();
        nodeMap[bb].sdom = bb;
        dfnVector.push_back(bb);
    });
    cfg.setParentHandler([&](BasicBlock *bb, BasicBlock *parent) {
        nodeMap[bb].parent = parent;
    });
    cfg.dfs(&F.getEntryBlock());

    for(int i = dfnVector.size() - 1; i >= 0; --i) {
        BasicBlock *bb = dfnVector[i];
        bb->print();
        for(auto *pred : predecessors(bb)) {
            BasicBlock *v = eval(pred);
            if(nodeMap[v].sdom < nodeMap[bb].sdom) {
                nodeMap[bb].sdom = nodeMap[v].sdom;
            }
        }
        nodeMap[nodeMap[bb].sdom].bucket.insert(bb);
        link(bb, nodeMap[bb].parent);
        if(!nodeMap[bb].parent) {// entry block
            continue;
        }
        auto &bucket = nodeMap[nodeMap[bb].parent].bucket;
        for (auto v : bucket) {
            std::cout << "fuck" << std::endl;
            BasicBlock *u = eval(v);
            __assert__(u && v, "u or v is nullptr");
            if(nodeMap[u].sdom < nodeMap[v].sdom) {
                DTResult.idomMap[v] = u;
            } else {
                DTResult.idomMap[v] = nodeMap[bb].parent;
            }
        }
        bucket.clear();
    }


    for(size_t i = 1; i < dfnVector.size(); ++i) {
        BasicBlock *bb = dfnVector[i];
        if(DTResult.idomMap[bb] != nodeMap[bb].sdom) {
            DTResult.idomMap[bb] = DTResult.idomMap[DTResult.idomMap[bb]];
        }
    }
    DTResult.idomMap[&F.getEntryBlock()] = nullptr;

    /// Build the dominator tree
    for (auto &bb : F) {
        if(DTResult.idomMap[&bb] != nullptr) {
            DTResult.childrenMap[DTResult.idomMap[&bb]].insert(&bb);
        }
    }
}







} // end namespace IR 