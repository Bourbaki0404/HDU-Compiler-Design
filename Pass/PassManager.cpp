#include "Pass/pass.h"
#include "IR/module.hpp"
#include "IR/Function.hpp"

bool PassManager::runFunctionPassOnModule(FunctionPass *P, IR::Module &M) {
    bool Changed = false;
    
    // Run the pass on each function in the module
    // This depends on your Module class implementation
    // Here's a simple implementation assuming Module has functions() or similar
    for (auto &F : M) {
        Changed |= P->runOnFunction(F);
    }
    
    return Changed;
} 