#pragma once
#include "common/common.hpp"
#include "common/dlist.hpp"
#include "IR/Type.hpp"
#include "common/iterator_range.hpp"
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

    // get the user
    Instruction *getUser() const {
        return inst;
    }

    // get the usee
    Value *getUsee() const {
        return val;
    }

    /// Return the operand # of this use in its User.
    unsigned getOperandNo() const ;
    
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

    class use_iterator {
        private:
            using dlist_iterator = typename dlist<Use>::iterator;
            dlist_iterator it;
            
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = Instruction;
            using pointer = Instruction*;
            using reference = Instruction&;
            
            use_iterator() = default;
            use_iterator(dlist_iterator _it) : it(_it) {}
            use_iterator(const use_iterator &other) : it(other.it) {}
            
            pointer operator*() const { return it->getUser(); }
            pointer operator->() const { return it->getUser(); }
            
            use_iterator& operator++() { ++it; return *this; }
            use_iterator operator++(int) { use_iterator tmp = *this; ++(*this); return tmp; }

            Use *getUse() const {
                return &*it;
            }

            bool atEnd() const {
                return it.atEnd();
            }
            
            bool operator==(const use_iterator& other) const { return it == other.it; }
            bool operator!=(const use_iterator& other) const { return !(*this == other); }
    };

    class const_use_iterator {
            using dlist_iterator = typename dlist<Use>::const_iterator;
            dlist_iterator it;
            
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = const Instruction;
            using pointer = const Instruction*;
            using reference = const Instruction&;

            const_use_iterator() = default;
            const_use_iterator(dlist_iterator _it) : it(_it) {}
            const_use_iterator(const const_use_iterator &other) : it(other.it) {}

            pointer operator*() const { return it->getUser(); }
            pointer operator->() const { return it->getUser(); }
            
            const_use_iterator& operator++() { ++it; return *this; }
            const_use_iterator operator++(int) { const_use_iterator tmp = *this; ++(*this); return tmp; }

            const Use *getUse() const {
                return &*it;
            }   

            bool atEnd() const {
                return it.atEnd();
            }
            
            bool operator==(const const_use_iterator& other) const { return it == other.it; }
            bool operator!=(const const_use_iterator& other) const { return !(*this == other); }
    };  

    using use_range = iterator_range<use_iterator>;
    using const_use_range = iterator_range<const_use_iterator>;

    use_iterator use_begin() {
        return use_iterator(useList.begin());
    }

    use_iterator use_end() {
        return use_iterator(useList.end());
    }

    const_use_iterator use_begin() const {
        return const_use_iterator(useList.begin());
    }

    const_use_iterator use_end() const {
        return const_use_iterator(useList.end());
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
