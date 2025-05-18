#pragma once
#include "IR/Function.hpp"
#include "IR/instruction.hpp"
namespace IR {

/// This provides the default implementation of the IRBuilder
/// 'InsertHelper' method that is called whenever an instruction is created by
/// IRBuilder and needs to be inserted.
/// By default, this inserts the instruction at the insertion point.
class IRBuilderDefaultInserter {
public:
  virtual ~IRBuilderDefaultInserter() {}

  virtual void InsertHelper(Instruction *I, 
                            BasicBlock *BB,
                            BasicBlock::iterator InsertPt) const {
    if (BB) BB->InstList.insert(InsertPt, I);
  }
};


struct IRBuilder {

    IRBuilder() {
        Inserter = new IRBuilderDefaultInserter;
        ClearInsertionPoint();
    }

    void ClearInsertionPoint() {
        BB = nullptr;
        InsertPt = BasicBlock::iterator();
    }

    /// This specifies that created instructions should be appended to the
    /// end of the specified block.
    void setInsertPoint(BasicBlock *blk) {
        BB = blk;
        InsertPt = blk->end();
    }

    /// This specifies that created instructions should be inserted before
    /// the specified instruction.
    void setInsertPoint(Instruction *I) {
        BB = I->parent;
        InsertPt = I->getIterator();
        assert(InsertPt != BB->end() && "Can't read debug loc from end()");
    }

    BasicBlock *getInsertBlock() {
        return BB;
    }

    BasicBlock::iterator getInsertPoint() {
        return InsertPt;
    }

    /// This specifies that created instructions should be inserted at the
    /// specified point.
    void SetInsertPoint(BasicBlock *TheBB, BasicBlock::iterator IP);

    template<typename InstTy>
    InstTy *Insert(InstTy *I) const {
        Inserter->InsertHelper(I, BB, InsertPt);
        return I;
    }

    // Integer arithmetic
    Value* CreateAdd(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::ADD, ty, lhs, rhs, name));
    }
    Value* CreateSub(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::SUB, ty, lhs, rhs, name));
    }
    Value* CreateMul(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::MUL, ty, lhs, rhs, name));
    }
    Value* CreateUDiv(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::UDIV, ty, lhs, rhs, name));
    }
    Value* CreateSDiv(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::SDIV, ty, lhs, rhs, name));
    }
    Value* CreateURem(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::UREM, ty, lhs, rhs, name));
    }
    Value* CreateSRem(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::SREM, ty, lhs, rhs, name));
    }



    // Floating-point arithmetic
    Value* CreateFAdd(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::FADD, ty, lhs, rhs, name));
    }
    Value* CreateFSub(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::FSUB, ty, lhs, rhs, name));
    }
    Value* CreateFMul(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::FMUL, ty, lhs, rhs, name));
    }
    Value* CreateFDiv(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::FDIV, ty, lhs, rhs, name));
    }
    Value* CreateFRem(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::FREM, ty, lhs, rhs, name));
    }

    // Logic 
    Value* CreateShl(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::SHL, ty, lhs, rhs, name));
    }
    Value* CreateLShr(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::LSHR, ty, lhs, rhs, name));
    }
    Value* CreateAShr(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::ASHR, ty, lhs, rhs, name));
    }
    Value* CreateAnd(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::AND, ty, lhs, rhs, name));
    }
    Value* CreateOr(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::OR, ty, lhs, rhs, name));
    }
    Value* CreateXor(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOp::BinaryOpKind::XOR, ty, lhs, rhs, name));
    }


    // // Integer comparison
    Value* CreateICmpEQ(Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new ICmpInst(ICmpInst::Predicate::ICMP_EQ, lhs, rhs, name));
    }
    Value* CreateICmpLT(Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new ICmpInst(ICmpInst::Predicate::ICMP_ULT, lhs, rhs, name));
    }
    Value* CreateICmpGT(Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new ICmpInst(ICmpInst::Predicate::ICMP_UGT, lhs, rhs, name));
    }

    // // Floating-point comparison
    Value* CreateFCmpOEQ(Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new FCmpInst(FCmpInst::Predicate::FCMP_OEQ, lhs, rhs, name));
    }
    Value* CreateFCmpOLT(Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new FCmpInst(FCmpInst::Predicate::FCMP_OLT, lhs, rhs, name));
    }
    Value* CreateFCmpOGT(Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new FCmpInst(FCmpInst::Predicate::FCMP_OGT, lhs, rhs, name));
    }

    // // GEP
    // Value* IRBuilder::CreateGEP(Type* elementType, Value* basePtr, Value* index, const std::string& name);
    // Value* IRBuilder::CreateGEP(Type* elementType, Value* basePtr, std::vector<Value*> indexes, const std::string& name);

    // // Memory ops
    AllocaInst* CreateAlloca(Type* ty, const std::string& name = "") {
        return Insert(new AllocaInst(ty, name));
    }
    LoadInst* CreateLoad(Type* ty, Value* ptr, const std::string& name = "") {
        return Insert(new LoadInst(ty, ptr, name));
    }
    StoreInst* CreateStore(Value* val, Value* ptr) {
        return Insert(new StoreInst(val, ptr));
    }

    // // Bit extension
    // Value *buildZExt(Value *operand, Type *toType, const std::string &name = "");
    // Value *buildSExt(Value *operand, Type *toType, const std::string &name = "");

    // // Control flow
    Value* CreateBr(BasicBlock* dest) {
        return Insert(new BranchInst(dest));
    }
    
    Value* CreateCondBr(Value* cond, BasicBlock* ifTrue, BasicBlock* ifFalse) {
        return Insert(new BranchInst(ifTrue, ifFalse, cond));
    }

    Value* CreateRet(Value* val) {
        return Insert(new ReturnInst(val));
    }

    Value* CreateRetVoid() {
        return Insert(new ReturnInst());
    }

    PHINode* CreatePHI(Type* ty, unsigned numReserved, const std::string& name = "") {
        return Insert(new PHINode(ty, numReserved, name));
    }

    template<typename T, typename... Args>
    T* CreateTracked(Args&&... args) {
        T* val = new T(std::forward<Args>(args)...);
        allocatedObj.push_back(static_cast<void*>(val));
        return val;
    }
public:
    

private:
    BasicBlock *BB;
    BasicBlock::iterator InsertPt;
    IRBuilderDefaultInserter *Inserter;
    // Track all dynamically allocated Value*
    std::vector<void*> allocatedObj;
};




}