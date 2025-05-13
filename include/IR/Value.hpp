#pragma once
#include "common/common.hpp"
#include "common/dlist.hpp"
#include "IR/Type.hpp"

namespace IR
{

struct Value;
struct Instruction;

struct Use : public dlist_node<Use>{
    Use(Value *val, Instruction *inst);
    Value *val;
    Instruction *inst;
};

struct Value {
    Value(Type *ty, unsigned scid)
    : type(ty), name(), subclassID(scid) {

    }

    /// Change the name of the value.
    ///
    /// Choose a new unique name if the provided name is taken.
    ///
    /// \param Name The new name; or "" if the value's name should be removed.
    void setName(const std::string &Name) {}

    std::string getName() const {
        return name;
    }

    bool hasName() const {
        return name != "";
    }

    Type *getType() const {
        return this->type;
    }

    void setType(Type *type) {
        this->type = type;
    }

    // RTTI for subclass of Value
    enum ValueTy {
        // Constants
        FunctionVal,
        GlobalAliasVal,
        GlobalIFuncVal,
        GlobalVariableVal,
        BlockAddressVal,
        ConstantExprVal,
        DSOLocalEquivalentVal, // Excluded from LLVM_C_API when LLVM_C_API is defined
        NoCFIValueVal,

        // Constant Aggregates
        ConstantArrayVal,
        ConstantStructVal,
        ConstantVectorVal,

        // Constant Data
        UndefValueVal,
        PoisonValueVal,
        ConstantAggregateZeroVal,
        ConstantDataArrayVal,
        ConstantDataVectorVal,
        ConstantIntVal,
        ConstantFPVal,
        ConstantPointerNullVal,
        ConstantTokenNoneVal,

        ConstantFirstVal = FunctionVal, 
        ConstantLastVal = ConstantTokenNoneVal, 
        ConstantDataFirstVal = UndefValueVal, 
        ConstantDataLastVal = ConstantTokenNoneVal,  
        ConstantAggregateFirstValVal = ConstantArrayVal,
        ConstantAggregateLastVal = ConstantVectorVal, 

        // Values
        ArgumentVal = ConstantLastVal + 1,
        BasicBlockVal,

        // Metadata and InlineAsm
        MetadataAsValueVal,
        InlineAsmVal,

        // Memory Values
        MemoryUseVal,
        MemoryDefVal,
        MemoryPhiVal,

        // Instructions
        InstructionVal,
        // Enum values starting at InstructionVal are reserved for Instructions opcode;
        // don't add new values here!
    };

    /// Return an ID for each concrete type of this object, used for RTTI.
    ///
    /// This is used to implement the classof checks. Also,
    /// note that for instructions, the Instruction's opcode is added to
    /// InstructionVal. So this means three things:
    ///  there is no value with code InstructionVal (no opcode==0).
    ///  there are more possible values for the value type than in ValueTy enum.
    ///  the InstructionVal enumerator must be the highest valued enumerator in
    ///   the ValueTy enum.
    unsigned getValueID() const {
        return subclassID;
    }

    IR::Type *type;
    std::string name;
    const unsigned subclassID;

    void addUse(Use* u) {   
        useList.push_back(u);
    }
    // void replaceAllUsesWith (Value *V) {  
    // }

    // tracks who uses this value
    dlist<Use> useList;
};



}
