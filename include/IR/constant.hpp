#pragma once
#include "common/common.hpp"
#include "IR/Value.hpp"

namespace IR{

// Constant may contain operands
struct Constant : public Value {
    static Constant *getIntegerValue(Type *Ty, const uint64_t V);
    static bool classof(const Value *V) {
        static_assert(ConstantFirstVal == 0, "V->getValueID() >= ConstantFirstVal always succeeds");
        return V->getValueID() <= ConstantLastVal;
    }
};

/// Base class for constants with no operands.
///
/// These constants have no operands; they represent their data directly.
/// Since they can be in use by unrelated modules (and are never based on
/// GlobalValues), it never makes sense to RAUW them.
struct ConstantData : public Constant {

};

//===----------------------------------------------------------------------===//
/// This is the shared class of boolean and integer constants. This class
/// represents both boolean and integral constants.
/// Class for constant integers.
struct ConstantInt final : public Constant {
    uint64_t v;

    ConstantInt(IntegerType *Ty, const uint64_t val) {
        setType(Ty);
        v = val;//?
    }
    
};


}