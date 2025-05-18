#pragma once

#include "common/common.hpp"
#include <typeindex>
#include "IR/utils.hpp"

namespace IR {
    class Function;
    class Module;
}

// Forward declarations
class Pass;
class PassManager;

// Base class for analysis results
class AnalysisResult {
public:
    virtual ~AnalysisResult() = default;
};

/// Base class for all IR passes
class Pass {
public:
    Pass() = default;
    Pass(const Pass &) = delete;
    Pass &operator=(const Pass &) = delete;
    virtual ~Pass() = default;

    /// Get analysis results from another pass
    template<typename AnalysisType>
    typename AnalysisType::Result *getAnalysisResult(IR::Function &F) const;

    template<typename AnalysisType>
    typename AnalysisType::Result *getAnalysisResult(IR::Module &M) const;
    
    /// Set the pass manager that owns this pass
    void setPassManager(PassManager *PM) { Manager = PM; }
    
    /// Get the pass manager that owns this pass
    PassManager *getPassManager() const { return Manager; }

    /// Check if this is a module pass
    virtual bool isModulePass() const { return false; }
    
    /// Check if this is a function pass
    virtual bool isFunctionPass() const { return false; }
    

protected:
    PassManager *Manager = nullptr;
};

//===----------------------------------------------------------------------===//
/// ModulePass class - This class is used to implement unstructured
/// interprocedural optimizations and analyses.  ModulePasses may do anything
/// they want to the program.
///
class ModulePass : virtual public Pass {
public:
    ModulePass() = default;
    virtual bool runOnModule(IR::Module &M) = 0;

    /// Override to identify as module pass
    bool isModulePass() const override { return true; }
};

/// FunctionPass class - This class is used to implement most local optimizations.
/// FunctionPasses only operate on a single function at a time.
class FunctionPass : virtual public Pass {
public:
    FunctionPass() = default;
    virtual bool runOnFunction(IR::Function &F) = 0;

    /// Override to identify as function pass
    bool isFunctionPass() const override { return true; }
};

/// Analysis pass interface - All analysis passes should inherit from this
/// and implement the getResult method
template<typename ResultType>
class AnalysisPass : virtual public Pass {
public:
    AnalysisPass() = default;
    using Result = ResultType;
    
    /// Get the result of this analysis pass
    virtual ResultType *getAnalysisResult() = 0;
};

/// The pass manager class that handles running passes in sequence
class PassManager {
public:
    /// Default constructor creates an empty pass manager
    PassManager() = default;
    
    /// Constructor that initializes with a list of passes
    PassManager(const std::vector<Pass*> &Passes) : PassVector(Passes) {
        // Set pass manager for all passes
        for (Pass *P : PassVector) {
            P->setPassManager(this);
        }
    }
    
    /// Add a pass to the pipeline
    template<typename PassType>
    void addPass(PassType *P) {
        P->setPassManager(this);
        PassVector.push_back(P);
    }
    
    /// Create and add a new pass to the pipeline
    template<typename PassType, typename... Args>
    PassType* createAndAddPass(Args&&... args) {
        PassType *P = new PassType(std::forward<Args>(args)...);
        addPass(P);
        return P;
    }
    
    /// Run all passes in sequence on a module
    bool run(IR::Module &M) {
        bool Changed = false;
        for (Pass *P : PassVector) {
            if (P->isModulePass()) {
                Changed |= dynamic_cast<ModulePass*>(P)->runOnModule(M);
            } else if (P->isFunctionPass()) {
                // Run function passes on each function in the module
                // Note: We rely on the module providing a way to access functions
                // This will need to be implemented based on your Module class
                Changed |= runFunctionPassOnModule(dynamic_cast<FunctionPass*>(P), M);
            }
        }
        return Changed;
    }
    
    /// Run all passes in sequence on a function
    bool run(IR::Function &F) {
        bool Changed = false;
        for (Pass *P : PassVector) {
            if (P->isFunctionPass()) {
                Changed |= dynamic_cast<FunctionPass*>(P)->runOnFunction(F);
            }
            // Skip module passes when running on a single function
        }
        return Changed;
    }
    
    /// Get analysis result from a pass
    template<typename AnalysisType>
    typename AnalysisType::Result *getAnalysisResult(IR::Function &F) {
        AnalysisType *AP = new AnalysisType();
        __assert__(AP->isFunctionPass(), "AnalysisType is not a function pass");
        AP->setPassManager(this);
        AP->runOnFunction(F);
        return AP->getAnalysisResult();
    }

    template<typename AnalysisType>
    typename AnalysisType::Result *getAnalysisResult(IR::Module &M) {
        AnalysisType *AP = new AnalysisType();
        __assert__(AP->isModulePass(), "AnalysisType is not a module pass");
        AP->setPassManager(this);
        AP->runOnModule(M);
        return AP->getAnalysisResult();
    }
    
private:
    /// Helper to run a function pass on all functions in a module
    bool runFunctionPassOnModule(FunctionPass *P, IR::Module &M);
    
    /// Vector of passes to run in sequence
    std::vector<Pass*> PassVector;
};

// Implementation of getAnalysis template method
template<typename AnalysisType>
typename AnalysisType::Result *Pass::getAnalysisResult(IR::Function &F) const {
    __assert__(Manager, "Pass has no manager");
    return Manager->getAnalysisResult<AnalysisType>(F);
}

template<typename AnalysisType>
typename AnalysisType::Result *Pass::getAnalysisResult(IR::Module &M) const {
    __assert__(Manager, "Pass has no manager");
    return Manager->getAnalysisResult<AnalysisType>(M);
}

