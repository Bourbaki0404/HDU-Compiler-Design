#include "IR/instruction.hpp"
#include "IR/basicBlock.hpp"
#include "IR/Value.hpp"
#include "IR/utils.hpp"

namespace IR{

const Module *Instruction::getModule() const {
    __assert__(getParent(), "Instruction getModule fail. basic block is not set");
    return  getParent()->getModule();
}

Module *Instruction::getModule() {
    __assert__(getParent(), "Instruction getModule fail. basic block is not set");
    return  getParent()->getModule();
}

void Instruction::setOperand(Value* v, size_t i) {
    if(i >= getNumOperands()) {
        std::cout << "setOperand Fail\n";
        exit(1);
    } else {
        auto use = new Use(v, this);
        uses[i] = use;
        v->addUse(use);
    }
}

Value *Instruction::getOperand(size_t i) const {
    if(i >= getNumOperands()) {
        std::cout << "getOperand Fail\n";
        exit(1);
    } else {
        return uses[i]->val;
    }
}

void Instruction::removeFromParent() {
    getParent()->getInstList().remove(getIterator());
}

std::string Instruction::getOpcodeName(size_t OpCode) {
    switch (OpCode) {
        // Terminators
        case RET:    return "ret";
        case BR:     return "br";

        // Standard binary operators
        case ADD: return "add";
        case FADD: return "fadd";
        case SUB: return "sub";
        case FSUB: return "fsub";
        case MUL: return "mul";
        case FMUL: return "fmul";
        case UDIV: return "udiv";
        case SDIV: return "sdiv";
        case FDIV: return "fdiv";
        case UREM: return "urem";
        case SREM: return "srem";
        case FREM: return "frem";
        case SHL: return "shl";
        case LSHR: return "lshr";
        case ASHR: return "ashr";
        case AND: return "and";
        case OR: return "or";
        case XOR: return "xor";

        // Unary operators
        case NEG: return "neg";
        case FNEG: return "fneg";

        // Memory instructions
        case ALLOCA: return "alloca";
        case LOAD: return "load";
        case STORE: return "store";
        case GET_ELEMENT_PTR: return "getelementptr";

        // Convert instructions
        case TRUNC: return "trunc";
        case ZEXT: return "zext";
        case SEXT: return "sext";
        case FPTRUNC: return "fptrunc";
        case FPEXT: return "fpext";
        case FPTOUI: return "fptoui";
        case FPTOSI: return "fptosi";
        case UITOFP: return "uitofp";
        case SITOFP: return "sitofp";
        case INTTOPTR: return "inttoptr";
        case PTRTOINT: return "ptrtoint";
        case BITCAST: return "bitcast";

        // Other instructions
        case ICMP: return "icmp";
        case FCMP: return "fcmp";
        case PHI: return "phi";
        case CALL: return "call";

        default: return "<Invalid operator>";
    }
}

BinaryOp::BinaryOp(BinaryOpKind kind, Type* ty, Value* lhs, Value* rhs, const std::string& name)
: Instruction(ty, kind, 2) {
    this->op_kind = kind;
    if(!ty || !lhs || !rhs) {
        std::cout << "BinaryOp Fail\n";
        exit(1);
    }
    if(!ty->equals(lhs->type) || !ty->equals(rhs->type)) { //type mismatch
        
    }
    this->setOperand(lhs, 0);
    this->setOperand(rhs, 1);
    this->name = name;
}

IR::UnaryOp::UnaryOp(UnaryOpKind kind, Type *ty, Value *operand, const std::string &name) 
: Instruction(ty, kind, 1){

}

std::string CmpInst::getPredicateName(Predicate Pred) {
    switch (Pred) {
        default:                   return "unknown";
        case FCmpInst::FCMP_FALSE: return "false";
        case FCmpInst::FCMP_OEQ:   return "oeq";
        case FCmpInst::FCMP_OGT:   return "ogt";
        case FCmpInst::FCMP_OGE:   return "oge";
        case FCmpInst::FCMP_OLT:   return "olt";
        case FCmpInst::FCMP_OLE:   return "ole";
        case FCmpInst::FCMP_ONE:   return "one";
        case FCmpInst::FCMP_ORD:   return "ord";
        case FCmpInst::FCMP_UNO:   return "uno";
        case FCmpInst::FCMP_UEQ:   return "ueq";
        case FCmpInst::FCMP_UGT:   return "ugt";
        case FCmpInst::FCMP_UGE:   return "uge";
        case FCmpInst::FCMP_ULT:   return "ult";
        case FCmpInst::FCMP_ULE:   return "ule";
        case FCmpInst::FCMP_UNE:   return "une";
        case FCmpInst::FCMP_TRUE:  return "true";
        case ICmpInst::ICMP_EQ:    return "eq";
        case ICmpInst::ICMP_NE:    return "ne";
        case ICmpInst::ICMP_SGT:   return "sgt";
        case ICmpInst::ICMP_SGE:   return "sge";
        case ICmpInst::ICMP_SLT:   return "slt";
        case ICmpInst::ICMP_SLE:   return "sle";
        case ICmpInst::ICMP_UGT:   return "ugt";
        case ICmpInst::ICMP_UGE:   return "uge";
        case ICmpInst::ICMP_ULT:   return "ult";
        case ICmpInst::ICMP_ULE:   return "ule";
    }
}


BranchInst::BranchInst(BasicBlock *IfTrue)
    : Instruction(TypeFactory::getVoidTy(), BR, 1) {
    if(!IfTrue) {
        std::cout << "BranchInst Fail.\n";
    }
    setOperand(IfTrue, 0);
}

BranchInst::BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond)
    : Instruction(TypeFactory::getVoidTy(), BR, 3) {
  // Assign in order of operand index to make use-list order predictable.
    this->setOperand(IfTrue, 0);
    this->setOperand(IfFalse, 1);
    this->setOperand(Cond, 2);
}

ReturnInst::ReturnInst(Value *retVal)
: Instruction(TypeFactory::getVoidTy(), RET, 1) {
    this->setOperand(retVal, 0);
}

ReturnInst::ReturnInst()
: Instruction(TypeFactory::getVoidTy(), RET, 0) {}


}