#pragma once

#include "common/common.hpp"

namespace IR {

struct Instruction;
struct BasicBlock;

struct TypePrinter {
    static void print(Type *Ty);
};

struct AsmWriter {
    static void writeOperand(const Value *Operand, bool PrintType);
    static void printInstruction(Instruction *inst);
    static void printBasicBlock(BasicBlock *BB);
    // static void printFunction(Function *fn);
    static void printInstructionLine(Instruction *inst);
};

}