#include "IR/asmWriter.hpp"
#include "IR/basicBlock.hpp"
#include "IR/out_stream.hpp"
#include "IR/Value.hpp"
#include "IR/Function.hpp"
#include "IR/instruction.hpp"
#include "IR/out_stream.hpp"
#include "IR/argument.hpp"
#include "IR/Cast.hpp"
#include "IR/Type.hpp"
#include "IR/Globals.hpp"
#include "IR/Value.hpp"

namespace IR{

// Helper function for constructing slotTracker
SlotTracker *createSlotTracker(const Value *V);


void BasicBlock::print() {
	__assert__(this, "BasicBlock print fail. null pointer");
	__assert__(this->getParent(), "BasicBlock print fail. Function not set");
	__assert__(this->getModule(), "BasicBlock print fail. Module not set");
	SlotTracker *S = createSlotTracker(this);
	AsmWriter writer(this->getModule(), S);
	writer.printBasicBlock(this);
}

void Function::print() {
	__assert__(this, "Function print fail, null pointer");
	__assert__(this->getParent(), "Function print fail. Module not set");
	SlotTracker *S = createSlotTracker(this);
	AsmWriter writer(this->getParent(), S);
	writer.printFunction(this);
}

void TypePrinting::print(Type *Ty) {
    switch (Ty->getTypeKind()) {
        case Type::typeKind::IntegerTy: {
            auto *IntTy = dyn_cast<IntegerType>(Ty);
            Out << "i" << IntTy->getBitWidth();
            return;
        }

        case Type::typeKind::FloatTy: {
            Out << "float";
            return;
        }

		case Type::typeKind::VoidTy: {
			Out << "void";
			return;
		}

        case Type::typeKind::PointerTy: {
            auto *PTy = static_cast<PointerType*>(Ty);
            // Assume we have a method to get pointee type
            Type *elementType = reinterpret_cast<Type*>(PTy->getSubclassData());
            print(elementType);
            Out << '*';
            return;
        }

        case Type::typeKind::ArrayTy: {
            auto *ATy = static_cast<ArrayType*>(Ty);
            // Assume ArrayType has getElementType() and getNumElements()
            Type *elementType = reinterpret_cast<Type*>(ATy->getSubclassData());
            size_t numElements = 0; // replace this with actual retrieval logic
            Out << '[' << numElements << " x ";
            print(elementType);
            Out << ']';
            return;
        }

        case Type::typeKind::FunctionTy: {
            auto *FTy = static_cast<FunctionType*>(Ty);
            // Assume FunctionType has getReturnType() and getParamTypes()
            Type *retTy = reinterpret_cast<Type*>(FTy->getSubclassData());
            std::vector<Type*> paramTypes; // replace with actual param list

            print(retTy);
            Out << " (";
            for (size_t i = 0; i < paramTypes.size(); ++i) {
                if (i > 0) Out << ", ";
                print(paramTypes[i]);
            }
            Out << ')';
            return;
        }

        default:
            Out << "<unknown type>";
            return;
    }
}

enum PrefixType {
	GlobalPrefix,
	ComdatPrefix,
	LabelPrefix,
	LocalPrefix,
	NoPrefix
};

inline void SlotTracker::initializeIfNeeded() {
//   if (TheModule) {
//     // processModule();      // Initialize the module-level slot table
//     TheModule = nullptr;
//   }

  if (TheFunction && !FunctionProcessed)
    processFunction();    // Initialize the function-level slot table
}

SlotTracker::SlotTracker(const Function *F)
    : TheFunction(F), TheModule(F->getParent()) {
		__assert__(F, "SlotTracker Initialization Fail, the function cannot be null.\n");
		__assert__(TheModule, "SlotTracker Initialization Fail, the Module cannot be null.\n");
	}

SlotTracker::SlotTracker(const Module *M)
    : TheModule(M) {
		__assert__(M, "SlotTracker Initialization Fail, the module cannot be null.\n");
	}

SlotTracker *createSlotTracker(const Value *V) {
	__assert__(V, "CreateSlotTracker Fail");
	if (const Argument *FA = dyn_cast<Argument>(V))
		return new SlotTracker(FA->getParent());

	if (const Instruction *I = dyn_cast<Instruction>(V))
		if (I->getParent())
		return new SlotTracker(I->getParent()->getParent());

	if (const BasicBlock *BB = dyn_cast<BasicBlock>(V))
		return new SlotTracker(BB->getParent());

	// if (const GlobalVariable *GV = dyn_cast<GlobalVariable>(V))
	// 	return new SlotTracker(GV->getParent());

	if (const Function *Func = dyn_cast<Function>(V))
		return new SlotTracker(Func);

	return nullptr;
}

int SlotTracker::getLocalSlot(const Value *V) {
	__assert__(!isa<Constant>(V), "getLocalSlot fail!, V cannot be Constant.");

	// Load the values into table lazily
	initializeIfNeeded();

	auto it = fMap.find(V);
	if(it != fMap.end()) return (*it).second;
	return -1;
}

int SlotTracker::getGlobalSlot(const GlobalObject *G) {
	// __assert__(!G)

	initializeIfNeeded();
	auto it = mMap.find(G);
	if(it != mMap.end()) return (*it).second; 
	return -1;
}

void SlotTracker::processFunction() {
	fMap.clear();
	fNext = 0;
	if(!TheFunction) {
		std::cout << "process Function Fail! Function cannot be null.\n";
		exit(1);
	}
	for(const auto arg : TheFunction->args()) {
		if(!arg->hasName()) {
			CreateFunctionSlot(arg);
		}
	}
	for(auto &BB : *TheFunction) {
		if(!BB.hasName()) {
			CreateFunctionSlot(&BB);
		}
		for(auto &I : BB) {
			// std::cout << "CALLED\n";
			if(!I.hasName()) {
				CreateFunctionSlot(&I);
			}
		}
	}
	FunctionProcessed = true;
}

void SlotTracker::CreateFunctionSlot(const Value *V) {
	__assert__(V, "CreateFunctionSlot Fail! Cannot insert a null value.");
	__assert__(!V->hasName(), "CreateFunctionSlot Fail! Cannot insert a named value");
	__assert__(fMap.find(V) == fMap.end(), "CreateFunctionSlot Fail! Reinsert the same value.");
	fMap[V] = fNext++;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             

void printIRNameWithoutPrefix(const std::string &Name) {
	assert(!Name.empty() && "Cannot get empty name!");

	// Scan the name to see if it needs quotes first.
	bool NeedsQuotes = isdigit(static_cast<unsigned char>(Name[0]));
	if (!NeedsQuotes) {
		for (unsigned char C : Name) {
		// By making this unsigned, the value passed in to isalnum will always be
		// in the range 0-255.  This is important when building with MSVC because
		// its implementation will assert.  This situation can arise when dealing
		// with UTF-8 multibyte characters.
		if (!isalnum(static_cast<unsigned char>(C)) && C != '-' && C != '.' &&
			C != '_') {
			NeedsQuotes = true;
			break;
		}
		}
	}

	// If we didn't need any quotes, just write out the name in one blast.
	if (!NeedsQuotes) {
		Out << Name;
		return;
	}

	// Okay, we need quotes.  Output the quotes and escape any scary characters as
	// needed.
	Out << '"';
	// printEscapedString(Name, OS);
	Out << '"';
}

/// Turn the specified name into an 'LLVM name', which is either prefixed with %
/// (if the string only contains simple characters) or is surrounded with ""'s
/// (if it has special chars in it). Print it out.
static void PrintLLVMName(const std::string &Name, PrefixType Prefix) {
	switch (Prefix) {
	case NoPrefix:
		break;
	case GlobalPrefix:
		Out << '@';
		break;
	case ComdatPrefix:
		Out << '$';
		break;
	case LabelPrefix:
		break;
	case LocalPrefix:
		Out << '%';
		break;
	}
	printIRNameWithoutPrefix(Name);
}

static void PrintLLVMName(const Value *V) {
	PrintLLVMName(V->getName(), isa<GlobalObject>(V) ? GlobalPrefix : LocalPrefix);
}

AsmWriter::AsmWriter(Module *M, SlotTracker *S) 
: TheModule(M), Machine(S), TypePrinter(nullptr) {
	TypePrinter = new TypePrinting;
}

// Only non-global constant value (like constantInt) will be passed in there
// Constant
void AsmWriter::WriteConstantInternal(const Constant* CV) {
	if (isa<ConstantInt>(CV)) {
		if (auto CI = dyn_cast<ConstantInt>(CV)) {
			if (CI->getType()->isIntegerTy(1)) {
				Out << CI->getValue() ? "true" : "false";
			}
			Out << CI->getValue();
		}
		return;
	}
}



// Full implementation of printing a Value as an operand with support for
// TypePrinting, etc.
// Non-global constant -> print directly the constant
// Global -> print with @
// Local -> print with %
void AsmWriter::WriteAsOperandInternal(const Value *V) {
    if (V->hasName()) {
		PrintLLVMName(V);
		return;
	}

	SlotTracker *cur_Machine = this->Machine;

    const Constant *CV = dyn_cast<Constant>(V);
    if (CV && !isa<GlobalObject>(CV)) {
        __assert__(TypePrinter, "Constants require TypePrinting!");
        WriteConstantInternal(CV);
        return;
    }

    char Prefix = '%';
    int Slot;
    if (const GlobalObject *GV = dyn_cast<GlobalObject>(V)) {
			Slot = cur_Machine->getGlobalSlot(GV);
			Prefix = '@';
        } else { // the value is a local

        	Slot = cur_Machine->getLocalSlot(V);

        	// If the local value didn't succeed, then we may be referring to a value
        	// from a different function.  Translate it, as this can happen when using
        	// address of blocks.
			if (Slot == -1) {
				if ((cur_Machine = createSlotTracker(V))) {
					Slot = cur_Machine->getLocalSlot(V);
					delete cur_Machine;
				}
			}
        }

    if (Slot != -1)
        Out << Prefix << Slot;
    else
        Out << "<badref>";
}

void AsmWriter::writeOperand(const Value *Operand, bool PrintType) {
    if (!Operand) {
        Out << "<null operand!>";
        return;
    }
    if (PrintType) {
        TypePrinter->print(Operand->getType());
        Out << ' ';
    }
    WriteAsOperandInternal(Operand);
}

void AsmWriter::printInstruction(const Instruction *inst) {
	// Print out indentation for an instruction.
  	Out << "  ";

	// The dest is always a local name
	if (inst->hasName()) {
		PrintLLVMName(inst);
		Out << " = ";
	} else if (!inst->getType()->isVoidTy()) {
		// Print out the def slot taken.
		int SlotNum = Machine->getLocalSlot(inst);
		if(SlotNum == -1) {
			Out << "<badref>";
		} else {
			Out << "%" << SlotNum;
		}
		Out << " = ";
	}

	// if (const CallInst *CI = dyn_cast<CallInst>(&I)) {
	// 	if (CI->isMustTailCall())
	// 	Out << "musttail ";
	// 	else if (CI->isTailCall())
	// 	Out << "tail ";
	// 	else if (CI->isNoTailCall())
	// 	Out << "notail ";
	// }

	// Print out the opcode...
	Out << inst->getOpcodeName();

	// Print out the compare instruction predicates
	if (isa<CmpInst>(inst)) {
		auto CI = dyn_cast<CmpInst>(inst);
		Out << ' ' << CmpInst::getPredicateName(CI->getPredicate());
	}

	// Print out the type of the operands...
	auto *Operand = inst->getNumOperands() ? inst->getOperand(0) : nullptr;

	if (isa<BranchInst>(inst) && dyn_cast<BranchInst>(inst)->isConditional()) {
		const auto BI = dyn_cast<BranchInst>(inst);
		Out << ' ';
		writeOperand(BI->getCondition(), true);
		Out << ", ";
		writeOperand(BI->getSuccessor(0), true);
		Out << ", ";
		writeOperand(BI->getSuccessor(1), true);
	} else if (const AllocaInst *AI = dyn_cast<AllocaInst>(inst)) {
		Out << ' ';
		TypePrinter->print(AI->getAllocatedType());

		// Explicitly write the array size if the code is broken, if it's an array
		// allocation, or if the type is not canonical for scalar allocations.  The
		// latter case prevents the type from mutating when round-tripping through
		// assembly.
		// if (!AI->getArraySize() || AI->isArrayAllocation() ||
		// 	!AI->getArraySize()->getType()->isIntegerTy(32)) {
		// Out << ", ";
		// writeOperand(AI->getArraySize(), true);
		// }
		if (AI->isAligned()) {
			Out << ", align " << AI->getAlign();
		}
	} else if (isa<ReturnInst>(inst) && !Operand) {
    	Out << " void"; 
	} else if (Operand) {   // Print the normal way.
		// if (const auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
		// 	Out << ' ';
		// 	TypePrinting::print(GEP->getSourceElementType());
		// 	Out << ',';
		// } else if (const auto *LI = dyn_cast<LoadInst>(&I)) {
		// 	Out << ' ';
		// 	TypePrinting::print(LI->getType());
		// 	Out << ',';
		// }

		// PrintAllTypes - Instructions who have operands of all the same type
		// omit the type from all but the first operand.  If the instruction has
		// different type operands (for example br), then they are all printed.
		bool PrintAllTypes = false;
		Type *TheType = Operand->getType();

		// Select, Store and ShuffleVector always print all types.
		if (isa<StoreInst>(inst) || isa<ReturnInst>(inst)) {
			PrintAllTypes = true;
		} else {
			for (unsigned i = 1, E = inst->getNumOperands(); i != E; ++i) {
				Operand = inst->getOperand(i);
				// note that Operand shouldn't be null, but the test helps make dump()
				// more tolerant of malformed IR
				if (Operand && Operand->getType() != TheType) {
					PrintAllTypes = true;    // We have differing types!  Print them all!
					break;
				}
			}
		}

		if (!PrintAllTypes) {
			Out << ' ';
			TypePrinter->print(TheType);
		}

		Out << ' ';
		for (unsigned i = 0, E = inst->getNumOperands(); i != E; ++i) {
			if (i) Out << ", ";
			writeOperand(inst->getOperand(i), PrintAllTypes);
		}
	}
}

void AsmWriter::printInstructionLine(const Instruction *inst) {
	printInstruction(inst);
	Out << "\n";
}

/// printArgument - This member is called for every argument that is passed into
/// the function.  Simply print it out
void AsmWriter::printArgument(const Argument *Arg) {
  // Output type...
  TypePrinter->print(Arg->getType());

  // Output parameter attributes list
//   if (Attrs.hasAttributes()) {
//     Out << ' ';
//     writeAttributeSet(Attrs);
//   }

  // Output name, if available...
  if (Arg->hasName()) {
    Out << ' ';
    PrintLLVMName(Arg);
  } else {
    int Slot = Machine->getLocalSlot(Arg);
    __assert__(Slot != -1, "expect argument in function here");
    Out << " %" << Slot;
  }
}

void AsmWriter::printFunction(const Function *fn) {
	__assert__(fn, "printFunction fail, nullpointer\n");
	
	Machine->incorporateFunction(fn);

	Out << "\n";
	Out << "define ";

	TypePrinter->print(fn->getReturnType());

	Out << " ";
	WriteAsOperandInternal(fn);
	Out << "(";

	for(auto &arg : fn->args()) {
		if(arg->getArgNo() > 0) Out << ", ";
		printArgument(arg);
	}
	Out << ") ";



	Out << "{\n";

	for(auto &BB : *fn) {
		printBasicBlock(&BB);
	}

	Out << "}\n";
}

void AsmWriter::printBasicBlock(const BasicBlock *BB) {
	bool isEntry = BB->getParent() && BB->isEntryBlock();
    if(BB->hasName()) {
		Out << "\n";
        PrintLLVMName(BB->getName(), LabelPrefix);
		Out << ":\n";
    } else if(!isEntry) {
		SlotTracker *curMachine = Machine;
		Out << "\n";
		int slotNum = curMachine->getLocalSlot(BB);
		if(slotNum == -1) {
			Out << "<badref>:\n";
		} else {
			Out << slotNum << ":\n";
		}
	}
    for(auto &inst : BB->InstList) {
        printInstructionLine(&inst);
    }
}

}