#pragma once
#include "common/common.hpp"

/// A trait for graph traversal algorithms
template <typename NodeTy>
struct Graph {


    /// Set the default handler for the graph traversal
    /// The default handler prints the debug info
    Graph() {
        m_handler = [](NodeTy *node) {
            std::cout << "Visiting node: " << node->getName() << "\n";
        };
    }

    /// Perform a depth-first search on the graph with a stack
    /// start from the entry node of the CFG
    void dfs(NodeTy *entry) {
        std::unordered_set<NodeTy *> visited;
        std::stack<NodeTy *> worklist;
        worklist.push(entry);
        while (!worklist.empty()) {
            NodeTy *node = worklist.top();
            m_handler(node);
            worklist.pop();
            // node->print();
            for (auto *succ : successors(node)) {
                if (visited.insert(succ).second) {
                    m_parentHandler(succ, node);
                    worklist.push(succ);
                }
            }
        }
    }


    /// Perform a post-order traversal on the graph
    /// The handler is called when a node is visited
    /// Visit a node after all its successors are visited


    void postOrder(NodeTy *entry) {
        std::vector<NodeTy *> order;
        postOrderHelper(entry, order);
    }

    /// Perform a post-order traversal on the graph
    void postOrderReverse(NodeTy *entry) {
        std::vector<NodeTy *> order;
        postOrderReverseHelper(entry, order);
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            m_handler(*it);
        }
    }

    /// Set the handler for the graph traversal
    /// Called when a node is visited
    void setHandler(std::function<void(NodeTy *)> handler) {
        m_handler = handler;
    }

    /// Get the handler for the graph traversal
    std::function<void(NodeTy *)> getHandler() const {
        return m_handler;
    }

    void setParentHandler(std::function<void(NodeTy *, NodeTy *)> handler) {
        m_parentHandler = handler;
    }

    std::function<void(NodeTy *, NodeTy *)> getParentHandler() const {
        return m_parentHandler;
    }

private:

    /// Generate the sequence of nodes in the post-order reverse traversal of the graph
    void postOrderHelper(NodeTy *node, std::vector<NodeTy *> &order) {
        std::unordered_set<NodeTy *> visited;
        std::stack<NodeTy *> worklist;
        worklist.push(node);
        while (!worklist.empty()) {
            NodeTy *node = worklist.top();
            worklist.pop();
            if (visited.insert(node).second) {
                worklist.push(node);
                for (auto *succ : successors(node)) {
                    worklist.push(succ);
                }
            } else {
                order.push_back(node);
            }
        }
    }

private:
    std::function<void(NodeTy *)> m_handler;
    std::function<void(NodeTy *, NodeTy *)> m_parentHandler;
};