#pragma once
#include "common/common.hpp"
#include "IR/basicBlock.hpp"
#include "IR/Value.hpp"

namespace IR{

struct Argument;

struct Function : public Value {
    using BasicBlockList = dlist<BasicBlock>;

    void setFuncArgs(Function *Func, std::vector<std::string> FuncArgs);
    Function *createFunc(Type *RetTy, std::vector<Type *>& Params, std::string Name, bool isVarArg = false);
    BasicBlockList &getBasicBlockList() { return basic_blocks; }
      /// Returns the FunctionType for me.
      
    // FunctionType *getFunctionType() const {
    //     return cast<FunctionType>(getValueType());
    // }

    BasicBlockList basic_blocks;
    std::vector<Argument*> arguments;
    // std::unique_ptr<ValueSymbolTable> SymTab;
};



















}