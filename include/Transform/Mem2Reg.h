#include "common/common.hpp"
#include "include/Pass/pass.h"

namespace IR{


/// Mem2Reg is a pass that promotes memory to register
/// It is a wrapper of the promoteMemoryToRegister function
class Mem2Reg : public FunctionPass {
public:
    bool runOnFunction(Function &F);





    
};



}