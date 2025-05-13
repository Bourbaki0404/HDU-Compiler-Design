#include "IR/Value.hpp"
#include "IR/basicBlock.hpp"
#include "IR/Function.hpp"
#include "IR/module.hpp"
#include "IR/argument.hpp"
namespace IR{

Use::Use(Value* v, Instruction* i) : val(v), inst(i) {

}

// void Value::addUse(Use *u)  { 

// }

// void Value::replaceAllUsesWith(Value *V)
// {

// }

static bool getSymTab(Value *V, ValueSymbolTable *&ST) {
  ST = nullptr;
  if (Instruction *I = dyn_cast<Instruction>(V)) {
    if (BasicBlock *P = I->getParent())
      if (struct Function *PP = P->getParent())
        ST = PP->getValueSymbolTable();
  } else if (BasicBlock *BB = dyn_cast<BasicBlock>(V)) {
    if (struct Function *P = BB->getParent())
      ST = P->getValueSymbolTable();
  } else if (Argument *A = dyn_cast<Argument>(V)) {
    if (struct Function *P = A->getParent())
      ST = P->getValueSymbolTable();
  } else {
    assert(isa<Constant>(V) && "Unknown value type!");
    return true;  // no name is setable for this.
  }
  return false;
}


}