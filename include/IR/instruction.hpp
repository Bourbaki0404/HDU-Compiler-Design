#pragma once
#include "common/common.hpp"
#include "IR/Value.hpp"
#include "IR/Cast.hpp"

namespace IR{

struct BasicBlock;

enum InstKind {
    BINOP, UNAOP,

    CMP,

    ALLOCA, LOAD, STORE, 

    CALL,

    BR, RET
};


struct Instruction : public Value, public dlist_node<Instruction> {
    enum opcode {
        START_OF_ALL,

        // Binary operators
        BINARY_START,
            // Integer arithmetic
            ADD, SUB, MUL, UDIV, SDIV, UREM, SREM,
            // Floating-point arithmetic
            FADD, FSUB, FMUL, FDIV, FREM,
            // Bitwise operations
            SHL, LSHR, ASHR, AND, OR, XOR,
        BINARY_END,

        // Unary operators
        UNARY_START,
            NEG, FNEG,
        UNARY_END,

        // Memory operations
        MEMORY_START,
            ALLOCA, LOAD, STORE, GET_ELEMENT_PTR,
        MEMORY_END,

        // Conversion operations
        CONVERSION_START,
            TRUNC, ZEXT, SEXT, FPTRUNC, FPEXT,
            FPTOUI, FPTOSI, UITOFP, SITOFP,
            INTTOPTR, PTRTOINT, BITCAST,
        CONVERSION_END,

        // Comparison operations
        COMPARISON_START,
            ICMP, FCMP,
        COMPARISON_END,

        

        // terminator operations
        TERMINATOR_START,
            RET, BR,
        TERMINATOR_END,

        END_OF_ALL,
    };
    Instruction(Type *ty, size_t opcode, size_t numOps) 
    : Value(ty, InstructionVal + opcode) {
        this->setType(ty);
        this->numOperands = numOps;
    }

    size_t getOpcode() const { return subclassID - InstructionVal; }
    static std::string getOpcodeName(size_t OpCode);
    std::string getOpcodeName() const { return getOpcodeName(getOpcode()); }
    void setOperand(Value* v, size_t i);
    Value *getOperand(size_t i);
    size_t getNumOperands() const { return useList.size(); }
        // Non-copyable
    Instruction(const Instruction&) = delete;
    Instruction& operator=(const Instruction&) = delete;

    bool isTerminator() const { return isTerminator(getOpcode()); }
    bool isUnaryOp() const { return isUnaryOp(getOpcode()); }
    bool isBinaryOp() const { return isBinaryOp(getOpcode()); }
    bool isIntDivRem() const { return isIntDivRem(getOpcode()); }
    bool isShift() const { return isShift(getOpcode()); }
    bool isCast() const { return isCast(getOpcode()); }

    static inline bool isCast(size_t OpCode) {
        return OpCode > CONVERSION_START && OpCode < CONVERSION_END;
    }

    static inline bool isTerminator(size_t OpCode) {
        return OpCode > TERMINATOR_START && OpCode < TERMINATOR_END;
    }

    static inline bool isUnaryOp(size_t Opcode) {
        return Opcode > UNARY_START && Opcode < UNARY_END;
    }
    static inline bool isBinaryOp(size_t Opcode) {
        return Opcode > BINARY_START && Opcode < BINARY_END;
    }

    static inline bool isIntDivRem(size_t Opcode) {
        return Opcode == UDIV || Opcode == SDIV || Opcode == UREM || Opcode == SREM;
    }

    /// Determine if the Opcode is one of the shift instructions.
    static inline bool isShift(size_t OPCODE) {
        return OPCODE >= SHL && OPCODE <= ASHR;
    }

    /// Return true if this is a logical shift left or a logical shift right.
    inline bool isLogicalShift() const {
        return getOpcode() == SHL || getOpcode() == LSHR;
    }

    /// Return true if this is an arithmetic shift right.
    inline bool isArithmeticShift() const {
        return getOpcode() == ASHR;
    }

    /// Determine if the Opcode is and/or/xor.
    static inline bool isBitwiseLogicOp(size_t OPCODE) {
        return OPCODE == AND || OPCODE == OR || OPCODE == XOR;
    }

    /// Return true if this is and/or/xor.
    inline bool isBitwiseLogicOp() const {
        return isBitwiseLogicOp(getOpcode());
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast:
    static bool classof(Value *V) {
        return V->getValueID() >= Value::InstructionVal;
    }


    std::vector<Use*> useList;
    size_t numOperands;
    BasicBlock *parent; 
};


struct BinaryOp : public Instruction {
    // Arithmetic Instructions
    enum BinaryOpKind {
        ADD = BINARY_START + 1, 
        SUB, MUL, UDIV, SDIV, UREM, SREM,
        FADD, FSUB, FMUL, FDIV, FREM,
        SHL, LSHR, ASHR, AND, OR, XOR
    };
    static bool classof(Instruction *I) {
        return I->isBinaryOp();
    }
    static bool classof(Value *V) {
        return isa<Instruction>(V) && classof(dyn_cast<Instruction>(V));
    }
    BinaryOp(BinaryOpKind kind, Type* ty, Value* lhs, Value* rhs, const std::string& name = "");
    BinaryOpKind op_kind;
};


struct UnaryOp : public Instruction {
    enum UnaryOpKind {
        NEG = UNARY_START + 1, 
        FNEG
    };
    UnaryOp(UnaryOpKind kind, Type* ty, Value* operand, const std::string& name);
    static bool classof(const Instruction *I) {
        return I->isUnaryOp();
    }
    static bool classof(Value *V) {
        return isa<Instruction>(V) && classof(dyn_cast<Instruction>(V));
    }
};

/// This class is the base class for the comparison instructions.
/// Abstract base class of comparison instructions.
struct CmpInst : public Instruction {
    CmpInst();
    /// This enumeration lists the possible predicates for CmpInst subclasses.
    /// Values in the range 0-31 are reserved for FCmpInst, while values in the
    /// range 32-64 are reserved for ICmpInst. This is necessary to ensure the
    /// predicate values are not overlapping between the classes.
    enum Predicate : size_t {
        // Opcode            U L G E    Intuitive operation
        FCMP_FALSE = 0, ///< 0 0 0 0    Always false (always folded)
        FCMP_OEQ = 1,   ///< 0 0 0 1    True if ordered and equal
        FCMP_OGT = 2,   ///< 0 0 1 0    True if ordered and greater than
        FCMP_OGE = 3,   ///< 0 0 1 1    True if ordered and greater than or equal
        FCMP_OLT = 4,   ///< 0 1 0 0    True if ordered and less than
        FCMP_OLE = 5,   ///< 0 1 0 1    True if ordered and less than or equal
        FCMP_ONE = 6,   ///< 0 1 1 0    True if ordered and operands are unequal
        FCMP_ORD = 7,   ///< 0 1 1 1    True if ordered (no nans)
        FCMP_UNO = 8,   ///< 1 0 0 0    True if unordered: isnan(X) | isnan(Y)
        FCMP_UEQ = 9,   ///< 1 0 0 1    True if unordered or equal
        FCMP_UGT = 10,  ///< 1 0 1 0    True if unordered or greater than
        FCMP_UGE = 11,  ///< 1 0 1 1    True if unordered, greater than, or equal
        FCMP_ULT = 12,  ///< 1 1 0 0    True if unordered or less than
        FCMP_ULE = 13,  ///< 1 1 0 1    True if unordered, less than, or equal
        FCMP_UNE = 14,  ///< 1 1 1 0    True if unordered or not equal
        FCMP_TRUE = 15, ///< 1 1 1 1    Always true (always folded)
        FIRST_FCMP_PREDICATE = FCMP_FALSE,
        LAST_FCMP_PREDICATE = FCMP_TRUE,
        BAD_FCMP_PREDICATE = FCMP_TRUE + 1,
        ICMP_EQ = 32,  ///< equal
        ICMP_NE = 33,  ///< not equal
        ICMP_UGT = 34, ///< size_t greater than
        ICMP_UGE = 35, ///< size_t greater or equal
        ICMP_ULT = 36, ///< size_t less than
        ICMP_ULE = 37, ///< size_t less or equal
        ICMP_SGT = 38, ///< signed greater than
        ICMP_SGE = 39, ///< signed greater or equal
        ICMP_SLT = 40, ///< signed less than
        ICMP_SLE = 41, ///< signed less or equal
        FIRST_ICMP_PREDICATE = ICMP_EQ,
        LAST_ICMP_PREDICATE = ICMP_SLE,
        BAD_ICMP_PREDICATE = ICMP_SLE + 1
    };
    static std::string getPredicateName(Predicate Pred);
    void setPredicate(Predicate pred) { this->pred = pred; }
    Predicate getPredicate() const { return pred; }
    Predicate pred;
    static bool classof(const Instruction *I) {
        return I->getOpcode() == Instruction::ICMP ||
            I->getOpcode() == Instruction::FCMP;
    }
    static bool classof(Value *V) {
        return isa<Instruction>(V) && classof(dyn_cast<Instruction>(V));
    }

};

struct ICmpInst : public CmpInst {
    ICmpInst(Type* ty, Predicate pre, Value* lhs, Value* rhs, const std::string& name = "");
};

struct FCmpInst : public CmpInst {
    FCmpInst(Type* ty, Predicate pre, Value* lhs, Value* rhs, const std::string& name = "");
};

// Memory
struct AllocaInst : public Instruction {
    AllocaInst(Type* ty, const std::string& name = "");
};

struct LoadInst : public Instruction {
    LoadInst(Type* ty, Value* ptr, const std::string& name = "");
};

struct StoreInst : public Instruction {
    StoreInst(Value* val, Value* ptr);
};



}