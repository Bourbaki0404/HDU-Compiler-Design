#include "common/common.hpp"

struct Module;
struct Function;

class Pass {
public:
};

//===----------------------------------------------------------------------===//
/// ModulePass class - This class is used to implement unstructured
/// interprocedural optimizations and analyses.  ModulePasses may do anything
/// they want to the program.
///
class ModulePass : public Pass {
public:
    virtual bool runOnModule(Module &M) = 0;
    virtual void getAnalysisUsage(AnalysisUsage &AU) const = 0;
};

class FunctionPass : public Pass {
public:
    virtual bool runOnFunction(Function &F) = 0;
    virtual void getAnalysisUsage(AnalysisUsage &AU) const = 0;
};