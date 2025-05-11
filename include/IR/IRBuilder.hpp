#pragma once
#include "IR/Function.hpp"

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
    void SetInsertPoint(Instruction *I) {
        BB = I->parent;
        // InsertPt = I->getIterator();
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
        return Insert(new BinaryOp(BinaryOpKind::ADD, ty, lhs, rhs, name));
    }
    Value* CreateSub(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::SUB, ty, lhs, rhs, name));
    }
    Value* CreateMul(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::MUL, ty, lhs, rhs, name));
    }
    Value* CreateUDiv(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::UDIV, ty, lhs, rhs, name));
    }
    Value* CreateSDiv(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::SDIV, ty, lhs, rhs, name));
    }
    Value* CreateURem(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::UREM, ty, lhs, rhs, name));
    }
    Value* CreateSRem(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::SREM, ty, lhs, rhs, name));
    }



    // Floating-point arithmetic
    Value* CreateFAdd(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::FADD, ty, lhs, rhs, name));
    }
    Value* CreateFSub(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::FSUB, ty, lhs, rhs, name));
    }
    Value* CreateFMul(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::FMUL, ty, lhs, rhs, name));
    }
    Value* CreateFDiv(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::FDIV, ty, lhs, rhs, name));
    }
    Value* CreateFRem(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::FREM, ty, lhs, rhs, name));
    }

    // Logic 
    Value* CreateShl(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::SHL, ty, lhs, rhs, name));
    }
    Value* CreateLShr(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::LSHR, ty, lhs, rhs, name));
    }
    Value* CreateAShr(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::ASHR, ty, lhs, rhs, name));
    }
    Value* CreateAnd(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::AND, ty, lhs, rhs, name));
    }
    Value* CreateOr(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::OR, ty, lhs, rhs, name));
    }
    Value* CreateXor(Type* ty, Value* lhs, Value* rhs, const std::string& name = "") {
        return Insert(new BinaryOp(BinaryOpKind::XOR, ty, lhs, rhs, name));
    }


    // // Integer comparison
    // Value* CreateICmpEQ(Type* ty, Value* lhs, Value* rhs, const std::string& name = "");
    // Value* CreateICmpLT(Type* ty, Value* lhs, Value* rhs, const std::string& name = "");
    // Value* CreateICmpGT(Type* ty, Value* lhs, Value* rhs, const std::string& name = "");

    // // Floating-point comparison
    // Value* CreateFCmpOEQ(Type* ty, Value* lhs, Value* rhs, const std::string& name = "");
    // Value* CreateFCmpOLT(Type* ty, Value* lhs, Value* rhs, const std::string& name = "");
    // Value* CreateFCmpOGT(Type* ty, Value* lhs, Value* rhs, const std::string& name = "");

    // // GEP
    // Value* IRBuilder::CreateGEP(Type* elementType, Value* basePtr, Value* index, const std::string& name);
    // Value* IRBuilder::CreateGEP(Type* elementType, Value* basePtr, std::vector<Value*> indexes, const std::string& name);

    // // Memory ops
    // Value* CreateAlloca(Type* ty, const std::string& name = "");
    // Value* CreateLoad(Type* ty, Value* ptr, const std::string& name = "");
    // Value* CreateStore(Value* val, Value* ptr);

    // // Bit extension
    // Value *buildZExt(Value *operand, Type *toType, const std::string &name = "");
    // Value *buildSExt(Value *operand, Type *toType, const std::string &name = "");

    // // Control flow
    // Value* CreateBr(BasicBlock* dest);
    // Value* CreateCondBr(Value* cond, BasicBlock* ifTrue, BasicBlock* ifFalse);
    // Value* CreateRet(Value* val);
    // Value* CreateRetVoid();

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