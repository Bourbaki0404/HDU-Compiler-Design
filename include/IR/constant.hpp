#pragma once
#include "common/common.hpp"
#include "IR/Value.hpp"

namespace IR{

// Constant may contain operands
// Constant can be named(globalObjects can have name) or unnamed(like constantInt)
struct Constant : public Value {
    Constant(Type *Ty, unsigned scid)
    : Value(Ty, scid) {}
    static Constant *getIntegerValue(Type *Ty, const uint64_t V);

    static bool classof(const Value *V) {
        static_assert(ConstantFirstVal == 0, "V->getValueID() >= ConstantFirstVal always succeeds");
        return ConstantFirstVal <= V->getValueID() && V->getValueID()<= ConstantLastVal;
    }
};


//////////////////////////////////////////////////////////////////////////////////////////////////ConstantData://////////////////////////////////
/// Constant data is a constant that has no name and can be used as the operand of instruction


//===----------------------------------------------------------------------===//
/// This is the shared class of boolean and integer constants. This class
/// represents both boolean and integral constants.
/// Class for constant integers.
struct ConstantInt final : public Constant {

    ConstantInt(IntegerType *Ty, const uint64_t val) 
    : Constant(Ty, ConstantIntVal) {
        v = val;//?
    }
    uint64_t v;

    uint64_t getValue() const { return v; }

    // RTTI for dyn_cast
    static bool classof(const Value *V) {
        return V->getValueID() == ConstantIntVal;
    }
};


}