#include "common/common.hpp"
#include "include/Pass/pass.h"

namespace IR{

class Mem2Reg : public FunctionPass {
public:
    bool runOnFunction(Function &F) override;





    
};



}