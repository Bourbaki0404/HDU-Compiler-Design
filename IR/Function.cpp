#include "IR/Function.hpp"
#include "IR/module.hpp"
#include "IR/argument.hpp"

namespace IR{

Function::Function(FunctionType *ty, LinkageTypes Linkage, const std::string &Name, Module *M)
: GlobalObject(ty, FunctionVal, Linkage, Name), NumArgs(ty->getNumParams()) {

    // create a symbol table for the function
    SymTab = std::make_unique<ValueSymbolTable>();

    // build the arguments vector if needed
    BuildLazyArguments();

    // push the function to the list of functions in the module and update module symbol table
    // Local names will not be added to the global symbol table
    if(M) {
        M->getFunctionList().push_back(this);
    }
}

void Function::BuildLazyArguments() {
    // Create the arguments vector, all arguments start out unnamed.
    auto *FT = getFunctionType();
    if (NumArgs > 0) {
        for (unsigned i = 0, e = NumArgs; i != e; ++i) {
            Type *ArgTy = FT->getParamType(i);
            __assert__(!ArgTy->isVoidTy(), "Cannot have void typed arguments!");
            arguments.push_back(new Argument(ArgTy, "", const_cast<Function *>(this), i));
        }
    }
}

}
