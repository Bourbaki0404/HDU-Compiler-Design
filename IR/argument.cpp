#include "IR/argument.hpp"
#include "IR/Value.hpp"
#include "IR/Function.hpp"

namespace IR {

Argument::Argument(Type *Ty, const std::string &Name, struct Function *F,
                    unsigned ArgNo) 
: Value(Ty, ValueTy::ArgumentVal), parent(F), ArgNo(ArgNo) {
    // setName(Name);
}

}