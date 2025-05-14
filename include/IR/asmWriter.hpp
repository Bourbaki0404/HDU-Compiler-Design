#pragma once

#include "common/common.hpp"

namespace IR {

struct Instruction;
struct BasicBlock;
struct Function;
struct Module;
struct GlobalObject;
struct Type;
struct Value;
struct Argument;
struct Constant;

struct TypePrinting {
    void print(Type *Ty);
};

//===----------------------------------------------------------------------===//
// SlotTracker Class: Enumerate slot numbers for unnamed values
//===----------------------------------------------------------------------===//
/// This class provides computation of slot numbers for Assembly writing.
struct SlotTracker {
    explicit SlotTracker(const Function *F);
    explicit SlotTracker(const Module *M);

	using ValueMap = std::unordered_map<const Value*, int>; 

	/// If you'd like to deal with a function instead of just a module, use
	/// this method to get its data into the SlotTracker.
	void incorporateFunction(const Function *F) {
		TheFunction = F;
        FunctionProcessed = false;
	}

	const Function *getFunction() const { return TheFunction; }

	void processFunction();
    // void processModule();

	/// Return the slot number of the specified value in it's type
  	/// plane.  If something is not in the SlotTracker, return -1.
  	int getLocalSlot(const Value *V);
    int getGlobalSlot(const GlobalObject* G);
private:
    
	void CreateFunctionSlot(const Value *V);

    void initializeIfNeeded();
	
	const Function *TheFunction = nullptr;
    bool FunctionProcessed = false;

    const Module *TheModule = nullptr;
    bool ModuleProcessed = false;

	/// mMap - The slot map for the module level data.
	ValueMap mMap;
	unsigned mNext = 0;

	/// fMap - The slot map for the function level data.
	ValueMap fMap;
	unsigned fNext = 0;
};


// AsmWriter is used to print well-formed IR program. 
// To print an entity correctly, one should carefully set the module and SlotTracker which holds the necessary context information.
struct AsmWriter {

    AsmWriter(Module *M, SlotTracker *S);

    void writeOperand(const Value *Operand, bool PrintType);
    void printInstruction(const Instruction *inst);
    void printBasicBlock(const BasicBlock *BB);
    void printFunction(const Function *fn);
    void printInstructionLine(const Instruction *inst);
    void printArgument(const Argument *Arg);

    const Module *TheModule = nullptr;
    SlotTracker *Machine = nullptr;
    TypePrinting *TypePrinter = nullptr;

private:
    void WriteAsOperandInternal(const Value *V);
    void WriteConstantInternal(const Constant *CV);
};

}