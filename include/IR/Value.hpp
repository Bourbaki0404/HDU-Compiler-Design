#pragma once
#include "common/common.hpp"
#include "common/dlist.hpp"
#include "IR/Type.hpp"

namespace IR
{

struct Value;
struct Instruction;
struct ValueSymbolTable;


// A use can be understand as an entry in the table of use-def relation
// Table entry:     Value (def) <-|   use   |->   Instruction (user)
// The use itself is organized in the useList maintained in the Value class,
// and the user of the value will simply hold the pointer to the corresponding use.
// 1) To add a use-def relation, simply create a new Use object, add it to the useList of the value and push the pointer of the use to the uses vector of the Instruction.
// 2) To remove a use-def relation, remove the Use object from the useList of the value and remove the corresponding operand of the Instruction.
struct Use : public dlist_node<Use>{
    Use(Value *val, Instruction *inst);

    void addToList(dlist_node<Use> *u) {
        insert_after(u);
    }

    // detach the use from the useList of the value.
    void removeFromList() {
        remove_self();
    }

    // get the usee
    Value *getUser() const {
        return val;
    }

    // get the user
    Instruction *getInst() const {
        return inst;
    }

    // get the usee
    Value *getUsee() const {
        return val;
    }
    
    Value *val; // the usee
    Instruction *inst; // the user
};

struct Value {
    friend struct ValueSymbolTable;

    Value(Type *ty, unsigned scid)
    : type(ty), name(), subclassID(scid) {

    }

    /// Change the name of the value.
    ///
    /// Choose a new unique name if the provided name is taken. Modify the symbol table if needed.
    ///
    /// \param Name The new name; or "" if the value's name should be removed.
    void setName(const std::string &Name);

    std::string getName() const {
        return name;
    }

private: // for internal use only
    // Simply set the name of the value, do not modify the symbol table.
    void __setName__(const std::string &Name) {
        name = Name;
    }

public:
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

    void addUse(Use* u) {   
        useList.push_back(u);
    }

    // void removeUse(Use* u) {
    //     useList.remove(u);
    // }

    using useListType = dlist<Use>;
    using use_iterator = useListType::iterator;
    using const_use_iterator = useListType::const_iterator;
    using use_range = iterator_range<use_iterator>;
    using const_use_range = iterator_range<const_use_iterator>;

    use_iterator use_begin() {
        return useList.begin();
    }

    use_iterator use_end() {
        return useList.end();
    }

    const_use_iterator use_begin() const {
        return useList.begin();
    }

    const_use_iterator use_end() const {
        return useList.end();
    }

    use_range uses() {
        return make_range(use_begin(), use_end());
    }

    const_use_range uses() const {
        return make_range(use_begin(), use_end());
    }

    Use *front() {
        return &useList.front();
    }

    const Use *front() const {
        return &useList.front();
    }

    Use *back() {
        return &useList.back();
    }

    const Use *back() const {
        return &useList.back();
    }

    bool empty() const {
        return useList.empty();
    }

    /// Change all uses of this to point to a new Value.
    ///
    /// Go through the uses list for this definition and make each use point to
    /// "V" instead of "this".  After this completes, 'this's use list is
    /// guaranteed to be empty.
    void replaceAllUsesWith (Value *V);

    IR::Type *type;
    std::string name;
    const unsigned subclassID;

    // a linked list that tracks who uses this value
    useListType useList;
};



}
