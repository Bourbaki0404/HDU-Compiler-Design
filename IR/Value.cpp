#include "IR/Value.hpp"

namespace IR{

Use::Use(Value* v, Instruction* i) : val(v), inst(i) {

}

// void Value::addUse(Use *u)  { 

// }

// void Value::replaceAllUsesWith(Value *V)
// {

// }

}