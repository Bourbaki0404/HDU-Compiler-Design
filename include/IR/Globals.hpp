#pragma once
#include "IR/constant.hpp"

namespace IR{

struct Module;


struct GlobalObject : public Constant {
    GlobalObject(Type *Ty, unsigned scid)
    : Constant(Ty, scid) {
    }

    /// The setParent() should only be called by the SymbolTableListTraits!
protected:
    void    setParent(Module *p)        { parent = p;    }
public:
    Module *getParent() const           { return parent; }
    Module *parent; // manage parent for all subclass
    
    // RTTI for dyn_cast and isa
    static bool classof(const Value *V) {
        return  V->getValueID() == Value::FunctionVal ||
                V->getValueID() == Value::GlobalVariableVal ||
                V->getValueID() == Value::GlobalIFuncVal;
    }

    
};


}