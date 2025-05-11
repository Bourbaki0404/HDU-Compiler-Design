#include "IR/asmWriter.hpp"
#include "IR/basicBlock.hpp"
#include "IR/out_stream.hpp"
#include "IR/Value.hpp"
#include "IR/out_stream.hpp"
#include "asmWriter.hpp"

namespace IR{

void TypePrinter::print(Type *Ty) {
    switch (Ty->kind) {
        case typeKind::Integer: {
            auto *IntTy = static_cast<IntegerType*>(Ty);
            Out << "i" << IntTy->getBitWidth();
            return;
        }

        case typeKind::Float: {
            Out << "float";
            return;
        }

        case typeKind::Pointer: {
            auto *PTy = static_cast<PointerType*>(Ty);
            // Assume we have a method to get pointee type
            Type *elementType = reinterpret_cast<Type*>(PTy->getSubclassData());
            print(elementType);
            Out << '*';
            return;
        }

        case typeKind::Array: {
            auto *ATy = static_cast<ArrayType*>(Ty);
            // Assume ArrayType has getElementType() and getNumElements()
            Type *elementType = reinterpret_cast<Type*>(ATy->getSubclassData());
            size_t numElements = 0; // replace this with actual retrieval logic
            Out << '[' << numElements << " x ";
            print(elementType);
            Out << ']';
            return;
        }

        case typeKind::Function: {
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
    OS << Name;
    return;
  }

  // Okay, we need quotes.  Output the quotes and escape any scary characters as
  // needed.
  OS << '"';
  printEscapedString(Name, OS);
  OS << '"';
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
	PrintLLVMName(V->getName(), isa<GlobalValue>(V) ? GlobalPrefix : LocalPrefix);
}

// Full implementation of printing a Value as an operand with support for
// TypePrinting, etc.
static void WriteAsOperandInternal(const Value *V,
                                   AsmWriterContext &WriterCtx) {
    if (V->hasName()) {
        PrintLLVMName(Out, V);
        return;
    }

    const Constant *CV = dyn_cast<Constant>(V);
    if (CV && !isa<GlobalValue>(CV)) {
        assert(WriterCtx.TypePrinter && "Constants require TypePrinting!");
        WriteConstantInternal(Out, CV, WriterCtx);
        return;
    }

    // if (const InlineAsm *IA = dyn_cast<InlineAsm>(V)) {
    //     Out << "asm ";
    //     if (IA->hasSideEffects())
    //     Out << "sideeffect ";
    //     if (IA->isAlignStack())
    //     Out << "alignstack ";
    //     // We don't emit the AD_ATT dialect as it's the assumed default.
    //     if (IA->getDialect() == InlineAsm::AD_Intel)
    //     Out << "inteldialect ";
    //     if (IA->canThrow())
    //     Out << "unwind ";
    //     Out << '"';
    //     printEscapedString(IA->getAsmString(), Out);
    //     Out << "\", \"";
    //     printEscapedString(IA->getConstraintString(), Out);
    //     Out << '"';
    //     return;
    // }

    // if (auto *MD = dyn_cast<MetadataAsValue>(V)) {
    //     WriteAsOperandInternal(Out, MD->getMetadata(), WriterCtx,
    //                         /* FromValue */ true);
    //     return;
    // }

    char Prefix = '%';
    int Slot;
    auto *Machine = WriterCtx.Machine;
    // If we have a SlotTracker, use it.
    if (Machine) {
        if (const GlobalValue *GV = dyn_cast<GlobalValue>(V)) {
        Slot = Machine->getGlobalSlot(GV);
        Prefix = '@';
        } else {
        Slot = Machine->getLocalSlot(V);

        // If the local value didn't succeed, then we may be referring to a value
        // from a different function.  Translate it, as this can happen when using
        // address of blocks.
        if (Slot == -1)
            if ((Machine = createSlotTracker(V))) {
            Slot = Machine->getLocalSlot(V);
            delete Machine;
            }
        }
    } else if ((Machine = createSlotTracker(V))) {
        // Otherwise, create one to get the # and then destroy it.
        if (const GlobalValue *GV = dyn_cast<GlobalValue>(V)) {
        Slot = Machine->getGlobalSlot(GV);
        Prefix = '@';
        } else {
        Slot = Machine->getLocalSlot(V);
        }
        delete Machine;
        Machine = nullptr;
    } else {
        Slot = -1;
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
        TypePrinter::print(Operand->getType());
        Out << ' ';
    }
    WriteAsOperandInternal(Operand);
}

void AsmWriter::printInstruction(Instruction *inst) {
	// Print out indentation for an instruction.
  	Out << "  ";

	if (inst->hasName()) {
		PrintLLVMName(inst);
		Out << " = ";
	} else if (!inst->getType()->isVoidTy()) {
		// Print out the def slot taken.
		int SlotNum = Machine.getLocalSlot(inst);
		if (SlotNum == -1)
			Out << "<badref> = ";
		else
			Out << '%' << SlotNum << " = ";
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
	if (inst->isCmp()) {
		auto CI = static_cast<CmpInst*>(inst);
		Out << ' ' << CmpInst::getPredicateName(CI->getPredicate());
	}

	// Print out the type of the operands...
	auto *Operand = inst->getNumOperands() ? inst->getOperand(0) : nullptr;

	if (inst-> && cast<BranchInst>(I).isConditional()) {
		const BranchInst &BI(cast<BranchInst>(I));
		Out << ' ';
		writeOperand(BI.getCondition(), true);
		Out << ", ";
		writeOperand(BI.getSuccessor(0), true);
		Out << ", ";
		writeOperand(BI.getSuccessor(1), true);
	}
}

void AsmWriter::printInstructionLine(Instruction *inst) {
	printInstruction(inst);
	Out << "\n";
}

void AsmWriter::printBasicBlock(BasicBlock *BB) {
    if(BB->name != "") {
        Out <<   BB->name << ":\n";
    }
    for(auto &inst : BB->InstList) {
        printInstructionLine(&inst);
    }
}

}