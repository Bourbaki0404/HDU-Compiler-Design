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


        operandList[i] = use;
        v->addUse(use);
    }
}

Value *Instruction::getOperand(size_t i) const {
    if(i >= getNumOperands()) {
        std::cout << "getOperand Fail\n";
        exit(1);
    } else {
        return operandList[i]->val;
    }
}

unsigned Instruction::getNumSuccessors() const {
    __assert__(isTerminator(), "Instruction getNumSuccessors fail\n");
    switch(getOpcode()) {
        case RET: return ((const ReturnInst*)this)->getNumSuccessors();
        case BR : return ((const BranchInst*)this)->getNumSuccessors();
        default: __assert__(false, "undef instruction at getNumSuccessors");
    }
}

BasicBlock *Instruction::getSuccessor(unsigned i) const {
    __assert__(isTerminator() && i <= getNumSuccessors(), "Instruction getSuccessor fail\n");
    switch(getOpcode()) {
        case RET: return ((const ReturnInst*)this)->getSuccessor(i);
        case BR : return ((const BranchInst*)this)->getSuccessor(i);
        default: __assert__(false, "undef instruction at getNumSuccessors");
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
    this->setName(name);    
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

CmpInst::CmpInst(Predicate pred, size_t opcode, Value *lhs, Value *rhs, const std::string& name)
: Instruction(lhs->getType(), opcode, 2) {
    __assert__(lhs->getType() == rhs->getType(), "CmpInst: lhs and rhs have different types");
    this->setOperand(lhs, 0);
    this->setOperand(rhs, 1);
    this->setName(name);
}

ICmpInst::ICmpInst(Predicate pred, Value* lhs, Value* rhs, const std::string& name)
: CmpInst(pred, ICMP, lhs, rhs, name) {}

FCmpInst::FCmpInst(Predicate pred, Value* lhs, Value* rhs, const std::string& name)
: CmpInst(pred, FCMP, lhs, rhs, name) {}

BranchInst::BranchInst(BasicBlock *IfTrue)
    : Instruction(Type::getVoidTy(), BR, 1) {
    if(!IfTrue) {
        std::cout << "BranchInst Fail.\n";
    }
    setOperand(IfTrue, 0);
}

BranchInst::BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond)
    : Instruction(Type::getVoidTy(), BR, 3) {
  // Assign in order of operand index to make use-list order predictable.
    this->setOperand(IfTrue, 0);
    this->setOperand(IfFalse, 1);
    this->setOperand(Cond, 2);
}


AllocaInst::AllocaInst(Type* ty, const std::string& name)
: Instruction(ty->getPointerTo(), ALLOCA, 0) {
    this->allocatedTy = ty;
}

LoadInst::LoadInst(Type* ty, Value* ptr, const std::string& name)
: Instruction(ty, LOAD, 1) {
    __assert__(ptr->getType()->isPointerTy(), "LoadInst: pointer is not a pointer");
    __assert__(ty->equals(dyn_cast<PointerType>(ptr->getType())->getElementType()), "LoadInst: pointee type mismatch");
    this->setOperand(ptr, 0);
    this->setName(name);
}

StoreInst::StoreInst(Value* val, Value* ptr)
: Instruction(Type::getVoidTy(), STORE, 2) {
    __assert__(ptr->getType()->isPointerTy(), "StoreInst: value is not a pointer");
    __assert__(dyn_cast<PointerType>(ptr->getType())->getElementType()->equals(val->getType()), "StoreInst: pointee type mismatch"); // might not 
    this->setOperand(val, 0);
    this->setOperand(ptr, 1);
}

ReturnInst::ReturnInst(Value *retVal)
: Instruction(Type::getVoidTy(), RET, 1) {
    this->setOperand(retVal, 0);
}

ReturnInst::ReturnInst()
: Instruction(Type::getVoidTy(), RET, 0) {}


PHINode::PHINode(Type *ty, size_t numReserved, const std::string& name)
: Instruction(ty, PHI, numReserved << 1) {
    this->setName(name);
    this->reservedSpace = numReserved;
    this->currentNumIncoming = 0;
}

void PHINode::setIncomingValue(unsigned i, Value *V) {
    __assert__(i < getNumOperands() / 2, "PHINode::setIncomingValue: invalid operand index");
    setOperand(V, i);
}

void PHINode::setIncomingBlock(unsigned i, BasicBlock *BB) {
    __assert__(i >= getNumOperands() / 2, "PHINode::setIncomingBlock: invalid operand index");
    setOperand(BB, i + reservedSpace);
}

Value *PHINode::getIncomingValue(unsigned i) const {
    __assert__(i < getNumOperands() / 2, "PHINode::getIncomingValue: invalid operand index");
    return getOperand(i);
}

BasicBlock *PHINode::getIncomingBlock(unsigned i) const {
    __assert__(i >= getNumOperands() / 2, "PHINode::getIncomingBlock: invalid operand index");
    BasicBlock *BB = dyn_cast<BasicBlock>(getOperand(i + reservedSpace));
    __assert__(BB, "PHINode::getIncomingBlock: incoming block is nullptr");
    return BB;
}

void PHINode::addIncoming(Value *V, BasicBlock *BB) {
    __assert__(V->getType() == getType(), "PHINode::addIncoming: value type mismatch");
    __assert__(BB, "PHINode::addIncoming: incoming block is nullptr");
    setIncomingValue(currentNumIncoming, V);
    setIncomingBlock(currentNumIncoming, BB);
    currentNumIncoming++;
}



}