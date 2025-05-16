#include "Pass/pass.h"
#include "Transform/Mem2Reg.h"
#include "Analysis/DominatorTree.h"
#include "IR/module.hpp"

using namespace IR;

// Example showing how to use the pass pipeline
void optimizeWithPipeline(Module &M) {
    // Create a pass manager
    PassManager PM;
    
    // Add analysis passes
    DominatorTree *DT = new DominatorTree();
    PM.addPass(DT);
    
    // Add transformation passes
    Mem2Reg *M2R = new Mem2Reg();
    PM.addPass(M2R);
    
    // Add more passes as needed
    // PM.addPass(new MyCustomPass());
    
    // Run the entire pipeline on the module
    bool Changed = PM.run(M);
    
    if (Changed) {
        std::cout << "Module was modified by the optimization pipeline." << std::endl;
    }
    
    // Clean up - depending on your memory management strategy
    // If PassManager doesn't take ownership of passes, you'll need to delete them
    delete DT;
    delete M2R;
}

// Alternative example using the createAndAddPass method
void optimizeWithFactoryMethods(Module &M) {
    PassManager PM;
    
    // Create and add passes in one step
    DominatorTree *DT = PM.createAndAddPass<DominatorTree>();
    Mem2Reg *M2R = PM.createAndAddPass<Mem2Reg>();
    
    // Run the pipeline
    PM.run(M);
    
    // Note: In this case, the PassManager is responsible for deleting the passes
    // You would need to implement a destructor for PassManager that cleans up the passes
}

// Example showing how to run specific passes on a single function
void optimizeSingleFunction(Function &F) {
    PassManager PM;
    
    // Add passes that should run on this function
    PM.addPass(new DominatorTree());
    PM.addPass(new Mem2Reg());
    
    // Run just on this function
    PM.run(F);
} 