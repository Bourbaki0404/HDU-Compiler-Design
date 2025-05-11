
#ifndef __DAG_H
#define __DAG_H

#include "common/common.hpp"
#include "ExtendedValueType.h"

namespace isel {

enum isd_op {

};

enum nodeType {
    /// EntryToken - This is the marker used to indicate the start of a region.
    EntryToken,

    /// TokenFactor - This node takes multiple tokens as input and produces a
    /// single token result. This is used to represent the fact that the operand
    /// operators are independent of each other.
    TokenFactor,

    /// Various leaf nodes.
    BasicBlock,
    VALUETYPE,
    CONDCODE,
    Register,
    RegisterMask,
    Constant,
    ConstantFP,
    GlobalAddress,
    GlobalTLSAddress,
    FrameIndex,
    JumpTable,
    ConstantPool,
    ExternalSymbol,
    BlockAddress,

    /// FRAMEADDR, RETURNADDR - These nodes represent llvm.frameaddress and
    /// llvm.returnaddress on the DAG.  These nodes take one operand, the index
    /// of the frame or return address to return.  An index of zero corresponds
    /// to the current function's frame or return address, an index of one to
    /// the parent's frame or return address, and so on.
    FRAMEADDR,
    RETURNADDR,

    /// ADDROFRETURNADDR - Represents the llvm.addressofreturnaddress intrinsic.
    /// This node takes no operand, returns a target-specific pointer to the
    /// place in the stack frame where the return address of the current
    /// function is stored.
    ADDROFRETURNADDR,

    /// SPONENTRY - Represents the llvm.sponentry intrinsic. Takes no argument
    /// and returns the stack pointer value at the entry of the current
    /// function calling this intrinsic.
    SPONENTRY,

    /// LOCAL_RECOVER - Represents the llvm.localrecover intrinsic.
    /// Materializes the offset from the local object pointer of another
    /// function to a particular local object passed to llvm.localescape. The
    /// operand is the MCSymbol label used to represent this offset, since
    /// typically the offset is not known until after code generation of the
    /// parent.
    LOCAL_RECOVER,

    /// READ_REGISTER, WRITE_REGISTER - This node represents llvm.register on
    /// the DAG, which implements the named register global variables extension.
    READ_REGISTER,
    WRITE_REGISTER,

    /// FRAME_TO_ARGS_OFFSET - This node represents offset from frame pointer to
    /// first (possible) on-stack argument. This is needed for correct stack
    /// adjustment during unwind.
    FRAME_TO_ARGS_OFFSET,

    /// EH_DWARF_CFA - This node represents the pointer to the DWARF Canonical
    /// Frame Address (CFA), generally the value of the stack pointer at the
    /// call site in the previous frame.
    EH_DWARF_CFA,

    /// OUTCHAIN = EH_RETURN(INCHAIN, OFFSET, HANDLER) - This node represents
    /// 'eh_return' gcc dwarf builtin, which is used to return from
    /// exception. The general meaning is: adjust stack by OFFSET and pass
    /// execution to HANDLER. Many platform-related details also :)
    EH_RETURN,

    /// RESULT, OUTCHAIN = EH_SJLJ_SETJMP(INCHAIN, buffer)
    /// This corresponds to the eh.sjlj.setjmp intrinsic.
    /// It takes an input chain and a pointer to the jump buffer as inputs
    /// and returns an outchain.
    EH_SJLJ_SETJMP,

    /// OUTCHAIN = EH_SJLJ_LONGJMP(INCHAIN, buffer)
    /// This corresponds to the eh.sjlj.longjmp intrinsic.
    /// It takes an input chain and a pointer to the jump buffer as inputs
    /// and returns an outchain.
    EH_SJLJ_LONGJMP,

    /// OUTCHAIN = EH_SJLJ_SETUP_DISPATCH(INCHAIN)
    /// The target initializes the dispatch table here.
    EH_SJLJ_SETUP_DISPATCH,

    /// TargetConstant* - Like Constant*, but the DAG does not do any folding,
    /// simplification, or lowering of the constant. They are used for constants
    /// which are known to fit in the immediate fields of their users, or for
    /// carrying magic numbers which are not values which need to be
    /// materialized in registers.
    TargetConstant,
    TargetConstantFP,

    /// TargetGlobalAddress - Like GlobalAddress, but the DAG does no folding or
    /// anything else with this node, and this is valid in the target-specific
    /// dag, turning into a GlobalAddress operand.
    TargetGlobalAddress,
    TargetGlobalTLSAddress,
    TargetFrameIndex,
    TargetJumpTable,
    TargetConstantPool,
    TargetExternalSymbol,
    TargetBlockAddress,

    MCSymbol,

    /// TargetIndex - Like a constant pool entry, but with completely
    /// target-dependent semantics. Holds target flags, a 32-bit index, and a
    /// 64-bit index. Targets can use this however they like.
    TargetIndex,

    /// RESULT = INTRINSIC_WO_CHAIN(INTRINSICID, arg1, arg2, ...)
    /// This node represents a target intrinsic function with no side effects.
    /// The first operand is the ID number of the intrinsic from the
    /// llvm::Intrinsic namespace.  The operands to the intrinsic follow.  The
    /// node returns the result of the intrinsic.
    INTRINSIC_WO_CHAIN,

    /// RESULT,OUTCHAIN = INTRINSIC_W_CHAIN(INCHAIN, INTRINSICID, arg1, ...)
    /// This node represents a target intrinsic function with side effects that
    /// returns a result.  The first operand is a chain pointer.  The second is
    /// the ID number of the intrinsic from the llvm::Intrinsic namespace.  The
    /// operands to the intrinsic follow.  The node has two results, the result
    /// of the intrinsic and an output chain.
    INTRINSIC_W_CHAIN,

    /// OUTCHAIN = INTRINSIC_VOID(INCHAIN, INTRINSICID, arg1, arg2, ...)
    /// This node represents a target intrinsic function with side effects that
    /// does not return a result.  The first operand is a chain pointer.  The
    /// second is the ID number of the intrinsic from the llvm::Intrinsic
    /// namespace.  The operands to the intrinsic follow.
    INTRINSIC_VOID,

    /// CopyToReg - This node has three operands: a chain, a register number to
    /// set to this value, and a value.
    CopyToReg,

    /// CopyFromReg - This node indicates that the input value is a virtual or
    /// physical register that is defined outside of the scope of this
    /// SelectionDAG.  The register is available from the RegisterSDNode object.
    CopyFromReg,

    /// UNDEF - An undefined node.
    UNDEF,

    // FREEZE - FREEZE(VAL) returns an arbitrary value if VAL is UNDEF (or
    // is evaluated to UNDEF), or returns VAL otherwise. Note that each
    // read of UNDEF can yield different value, but FREEZE(UNDEF) cannot.
    FREEZE,

    /// EXTRACT_ELEMENT - This is used to get the lower or upper (determined by
    /// a Constant, which is required to be operand #1) half of the integer or
    /// float value specified as operand #0.  This is only for use before
    /// legalization, for values that will be broken into multiple registers.
    EXTRACT_ELEMENT,

    /// BUILD_PAIR - This is the opposite of EXTRACT_ELEMENT in some ways.
    /// Given two values of the same integer value type, this produces a value
    /// twice as big.  Like EXTRACT_ELEMENT, this can only be used before
    /// legalization. The lower part of the composite value should be in
    /// element 0 and the upper part should be in element 1.
    BUILD_PAIR,

    /// MERGE_VALUES - This node takes multiple discrete operands and returns
    /// them all as its individual results.  This nodes has exactly the same
    /// number of inputs and outputs. This node is useful for some pieces of the
    /// code generator that want to think about a single node with multiple
    /// results, not multiple nodes.
    MERGE_VALUES,

    /// Simple integer binary arithmetic operators.
    ADD,
    SUB,
    MUL,
    SDIV,
    UDIV,
    SREM,
    UREM,

    /// SMUL_LOHI/UMUL_LOHI - Multiply two integers of type iN, producing
    /// a signed/unsigned value of type i[2*N], and return the full value as
    /// two results, each of type iN.
    SMUL_LOHI,
    UMUL_LOHI,

    /// SDIVREM/UDIVREM - Divide two integers and produce both a quotient and
    /// remainder result.
    SDIVREM,
    UDIVREM,

    /// CARRY_FALSE - This node is used when folding other nodes,
    /// like ADDC/SUBC, which indicate the carry result is always false.
    CARRY_FALSE,

    /// Carry-setting nodes for multiple precision addition and subtraction.
    /// These nodes take two operands of the same value type, and produce two
    /// results.  The first result is the normal add or sub result, the second
    /// result is the carry flag result.
    /// FIXME: These nodes are deprecated in favor of ADDCARRY and SUBCARRY.
    /// They are kept around for now to provide a smooth transition path
    /// toward the use of ADDCARRY/SUBCARRY and will eventually be removed.
    ADDC,
    SUBC,

    /// Carry-using nodes for multiple precision addition and subtraction. These
    /// nodes take three operands: The first two are the normal lhs and rhs to
    /// the add or sub, and the third is the input carry flag.  These nodes
    /// produce two results; the normal result of the add or sub, and the output
    /// carry flag.  These nodes both read and write a carry flag to allow them
    /// to them to be chained together for add and sub of arbitrarily large
    /// values.
    ADDE,
    SUBE,

    /// Carry-using nodes for multiple precision addition and subtraction.
    /// These nodes take three operands: The first two are the normal lhs and
    /// rhs to the add or sub, and the third is a boolean indicating if there
    /// is an incoming carry. These nodes produce two results: the normal
    /// result of the add or sub, and the output carry so they can be chained
    /// together. The use of this opcode is preferable to adde/sube if the
    /// target supports it, as the carry is a regular value rather than a
    /// glue, which allows further optimisation.
    ADDCARRY,
    SUBCARRY,

    /// Carry-using overflow-aware nodes for multiple precision addition and
    /// subtraction. These nodes take three operands: The first two are normal lhs
    /// and rhs to the add or sub, and the third is a boolean indicating if there
    /// is an incoming carry. They produce two results: the normal result of the
    /// add or sub, and a boolean that indicates if an overflow occured (*not*
    /// flag, because it may be a store to memory, etc.). If the type of the
    /// boolean is not i1 then the high bits conform to getBooleanContents.
    SADDO_CARRY,
    SSUBO_CARRY,

    /// RESULT, BOOL = [SU]ADDO(LHS, RHS) - Overflow-aware nodes for addition.
    /// These nodes take two operands: the normal LHS and RHS to the add. They
    /// produce two results: the normal result of the add, and a boolean that
    /// indicates if an overflow occurred (*not* a flag, because it may be store
    /// to memory, etc.).  If the type of the boolean is not i1 then the high
    /// bits conform to getBooleanContents.
    /// These nodes are generated from llvm.[su]add.with.overflow intrinsics.
    SADDO,
    UADDO,

    /// Same for subtraction.
    SSUBO,
    USUBO,

    /// Same for multiplication.
    SMULO,
    UMULO,

    /// RESULT = [US]ADDSAT(LHS, RHS) - Perform saturation addition on 2
    /// integers with the same bit width (W). If the true value of LHS + RHS
    /// exceeds the largest value that can be represented by W bits, the
    /// resulting value is this maximum value. Otherwise, if this value is less
    /// than the smallest value that can be represented by W bits, the
    /// resulting value is this minimum value.
    SADDSAT,
    UADDSAT,

    /// RESULT = [US]SUBSAT(LHS, RHS) - Perform saturation subtraction on 2
    /// integers with the same bit width (W). If the true value of LHS - RHS
    /// exceeds the largest value that can be represented by W bits, the
    /// resulting value is this maximum value. Otherwise, if this value is less
    /// than the smallest value that can be represented by W bits, the
    /// resulting value is this minimum value.
    SSUBSAT,
    USUBSAT,

    /// RESULT = [US]SHLSAT(LHS, RHS) - Perform saturation left shift. The first
    /// operand is the value to be shifted, and the second argument is the amount
    /// to shift by. Both must be integers of the same bit width (W). If the true
    /// value of LHS << RHS exceeds the largest value that can be represented by
    /// W bits, the resulting value is this maximum value, Otherwise, if this
    /// value is less than the smallest value that can be represented by W bits,
    /// the resulting value is this minimum value.
    SSHLSAT,
    USHLSAT,

    /// RESULT = [US]MULFIX(LHS, RHS, SCALE) - Perform fixed point multiplication
    /// on 2 integers with the same width and scale. SCALE represents the scale
    /// of both operands as fixed point numbers. This SCALE parameter must be a
    /// constant integer. A scale of zero is effectively performing
    /// multiplication on 2 integers.
    SMULFIX,
    UMULFIX,

    /// Same as the corresponding unsaturated fixed point instructions, but the
    /// result is clamped between the min and max values representable by the
    /// bits of the first 2 operands.
    SMULFIXSAT,
    UMULFIXSAT,

    /// RESULT = [US]DIVFIX(LHS, RHS, SCALE) - Perform fixed point division on
    /// 2 integers with the same width and scale. SCALE represents the scale
    /// of both operands as fixed point numbers. This SCALE parameter must be a
    /// constant integer.
    SDIVFIX,
    UDIVFIX,

    /// Same as the corresponding unsaturated fixed point instructions, but the
    /// result is clamped between the min and max values representable by the
    /// bits of the first 2 operands.
    SDIVFIXSAT,
    UDIVFIXSAT,
    FADD,
    FSUB,
    FMUL,
    FDIV,
    FREM
};

// Represents one node in the miniSelectionDAG.
struct sd_node {

    /// The operation that this node performs.
    int16_t nodeOp;

    /// Unique id per SDNode in the DAG.
    int NodeId = -1;
    /// The values that are used by this operation.
    sd_use *operandList = nullptr;
    /// List of uses for this SDNode.
    sd_use *UseList = nullptr;   
};

//===----------------------------------------------------------------------===//
/// Unlike LLVM values, Selection DAG nodes may return multiple
/// values as the result of a computation.  Many nodes return multiple values,
/// from loads (which define a token and a return value) to ADDC (which returns
/// a result and a carry value), to calls (which may return an arbitrary number
/// of values).
///
/// As such, each use of a SelectionDAG computation must indicate the node that
/// computes it as well as which return value to use from that node.  This pair
/// of information is represented with the SDValue value type.
struct sd_value {
    sd_node *Node = nullptr; // The node defining the value we are using.
    unsigned ResNo = 0;     // Which return value of the node we are using.
};

struct sd_use {
    /// Val - The value being used.
    sd_value Val;
    /// User - The user of this value.
    sd_node *User = nullptr;

    /// Prev, Next - Pointers to the uses list of the SDNode referred by
    /// this operand.
    sd_use **Prev = nullptr;
    sd_use *Next = nullptr;

    void addToList(sd_use **List) {
        Next = *List;
        if (Next) Next->Prev = &Next;
        Prev = List;
        *List = this;
    }

    void removeFromList() {
        *Prev = Next;
        if (Next) Next->Prev = Prev;
    }
};



}

struct selection_dag {

};

//===----------------------------------------------------------------------===//
/// SelectionDAGBuilder - This is the common target-independent lowering
/// implementation
struct selection_dag_builder {
    /// The current instruction being visited.
    const llvm::Instruction *CurInst = nullptr;

    selection_dag *DAG;
};


#endif