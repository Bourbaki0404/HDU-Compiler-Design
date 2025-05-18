#pragma once
#include "IR/constant.hpp"

namespace IR{

struct Module;

// The globalObject represents a pointer that points to the location of the global object, thus its type is of pointer type
struct GlobalObject : public Constant {
    friend struct Module;  

    /// An enumeration for the kinds of linkage for global values.
    enum LinkageTypes {
        ExternalLinkage = 0,///< Externally visible function
        AvailableExternallyLinkage, ///< Available for inspection, not emission.
        LinkOnceAnyLinkage, ///< Keep one copy of function when linking (inline)
        LinkOnceODRLinkage, ///< Same, but only replaced by something equivalent.
        WeakAnyLinkage,     ///< Keep one copy of named function when linking (weak)
        WeakODRLinkage,     ///< Same, but only replaced by something equivalent.
        AppendingLinkage,   ///< Special purpose, only applies to global arrays
        InternalLinkage,    ///< Rename collisions when linking (static functions).
        PrivateLinkage,     ///< Like Internal, but omit from symbol table.
        ExternalWeakLinkage,///< ExternalWeak linkage description.
        CommonLinkage       ///< Tentative definitions.
    };

    /// An enumeration for the kinds of visibility of global values.
    // enum VisibilityTypes {
    //     DefaultVisibility = 0,  ///< The GV is visible
    //     HiddenVisibility,       ///< The GV is hidden
    //     ProtectedVisibility     ///< The GV is protected
    // };
    

    GlobalObject(Type *Ty, unsigned scid, LinkageTypes Linkage, const std::string &Name)
    : Constant(Ty, scid), valueType(Ty), linkage(Linkage) {
        parent = nullptr;
        setName(Name);
    }

    Type *valueType;
    LinkageTypes linkage;
    Type *getValueType() const { return valueType; }

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