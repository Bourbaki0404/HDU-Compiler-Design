#include "IR/Value.hpp"
#include "IR/basicBlock.hpp"
#include "IR/Function.hpp"
#include "IR/module.hpp"
#include "IR/argument.hpp"
#include "IR/utils.hpp"
namespace IR{

Use::Use(Value* v, Instruction* i) : val(v), inst(i) {

}

// void Value::addUse(Use *u)  { 

// }

// void Value::replaceAllUsesWith(Value *V)
// {

// }


/// Get the symbol table associated with the value.
/// If the value is local, then the symbol table is the function's symbol table
/// If the value is global, then the symbol table is the module's symbol table
/// If the value is a constant data, then return nullptr
/// return true if the value is in the symbol table, false otherwise
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
    } else if(GlobalObject *GO = dyn_cast<GlobalObject>(V)) { // Function, GlobalVariable
		if(struct Module *M = GO->getParent())
			ST = M->getValueSymbolTable();
	} else {
        assert(isa<Constant>(V) && "Unknown value type!");
        return false;  // no name is setable for this.
    }
    return true;
}

// void Value::replaceAllUsesWith(Value *V) {
// 	for(Use &U : useList) {
// 		U.val = V;
// 	}
// }

unsigned Use::getOperandNo() const {
    for(unsigned i = 0; i < inst->getNumOperands(); i++) {
        if(inst->getOperand(i) == val) {
            return i;
        }
    }
}

void Value::setName(const std::string &Name) {
	// if the name is the same, do nothing
    if(Name == getName()) {
		return;
    }

	__assert__(!getType()->isVoidTy(), "Cannot assign a name to void values!");


	// only set the name of the value, do not modify the symbol table
	__setName__(Name);

	// get the symbol table
	ValueSymbolTable *ST = nullptr;
	if(!getSymTab(this, ST)) {
		std::cout << "Value::setName: " << Name << " is constant data, no symbol table associated with it" << std::endl;
		return;
	} /// if the value is a constant

	if(ST) { // if the value is in a symbol table
		//update the symbol table
		if(hasName()) {
			ST->removeValueName(getName());
			ST->reinsertValue(this);
		}
	} // if not in the symbol table, do nothing is ok
}


}