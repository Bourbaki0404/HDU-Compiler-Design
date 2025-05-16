#include "include/Pass/pass.h"
#include "include/Transform/Mem2Reg.h"
#include "include/Analysis/DominatorTree.h"
#include "include/IR/Module.hpp"
#include "include/IR/Function.hpp"

using namespace IR;

// Example of how to use the pass manager
void runPasses(Module &M) {
    // Create a pass manager
    PassManager PM;
    
    // Run the Mem2Reg pass on each function in the module
    for (Function &F : M) {
        // First run the DominatorTree analysis
        PM.run<DominatorTree>(F);
        
        // Get the analysis result
        DominatorTreeResult *DTR = PM.getAnalysis<DominatorTree>();
        if (DTR) {
            // We can use the analysis result directly
            const auto &DomFrontiers = DTR->getDomFrontiers();
            for (const auto &DF : DomFrontiers) {
                BasicBlock *BB = DF.first;
                std::cout << "Dominance frontier for BB " << BB->getName() << ": ";
                for (BasicBlock *Frontier : DF.second) {
                    std::cout << Frontier->getName() << " ";
                }
                std::cout << std::endl;
            }
        }
        
        // Run the Mem2Reg pass, which will automatically use the DominatorTree analysis
        bool Changed = PM.run<Mem2Reg>(F);
        if (Changed) {
            std::cout << "Function " << F.getName() << " was modified by Mem2Reg" << std::endl;
        }
    }
    
    // If we need to run more passes, we can invalidate all analysis results first
    PM.invalidateAll();
}

// Alternative way to run passes in a more declarative way
void runPassesDeclarative(Module &M) {
    // Create a pass manager
    PassManager PM;
    
    // Run all function passes on all functions
    for (Function &F : M) {
        // Run multiple passes in sequence
        std::vector<bool> Results = {
            PM.run<DominatorTree>(F),
            PM.run<Mem2Reg>(F)
            // Add more function passes here
        };
        
        // Check if any passes modified the function
        bool Changed = std::any_of(Results.begin(), Results.end(), [](bool b) { return b; });
        if (Changed) {
            std::cout << "Function " << F.getName() << " was modified" << std::endl;
        }
    }
} 