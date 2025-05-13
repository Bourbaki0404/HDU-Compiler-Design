#include "IR/Function.hpp"
#include "IR/module.hpp"

namespace IR{

Function::Function(FunctionType *ty, Module *M)
: GlobalObject(ty, FunctionVal) {
    if(M) {
        M->getFunctionList().push_back(this);
    }
}

}