#pragma once
#include "common/common.hpp"
#include "IR/symbolTableListTraits.hpp"
#include "IR/instruction.hpp"
#include "IR/constant.hpp"
#include "IR/asmWriter.hpp"

namespace IR{

struct ValueSymbolTable;
struct Function;
struct Module;

struct BasicBlock : public Value, public dlist_node<BasicBlock> {
	friend class SymbolTableListTraits<BasicBlock>;
	/// Constructor.
	///
	/// If the function parameter is specified, the basic block is automatically
	/// inserted at either the end of the function (if InsertBefore is null), or
	/// before the specified basic block.
	BasicBlock() 
	: Value(TypeFactory::getLabelTy(), BasicBlockVal) { parent = nullptr; }
	BasicBlock(const BasicBlock &) = delete;
	BasicBlock &operator=(const BasicBlock &) = delete;
	std::string label;
	struct Function *parent;
	using InstListType = SymbolTableList<Instruction>;

	ValueSymbolTable *getValueSymbolTable();

	bool isEntryBlock() const ;

	/// Return the underlying instruction list container.
	///
	/// Currently you need to access the underlying instruction list container
	/// directly if you want to modify it.
	const InstListType &getInstList() const { return InstList; }
	InstListType &getInstList()       { return InstList; }
	/// Returns a pointer to a member of the instruction list.
	static InstListType BasicBlock::*getSublistAccess(Instruction*) {
		return &BasicBlock::InstList;
	}
	
	/// The setParent() should only be called by the SymbolTableListTraits
protected:
	void 						  setParent(struct Function *parent);
public:
	inline const struct Function *getParent() const 	{ return parent; }
	inline struct Function *getParent()       			{ return parent; }
	const Module *				  getModule() const ;
	Module *					  getModule();
	//
	using iterator = InstListType::iterator;
	using const_iterator = InstListType::const_iterator;
	/// Instruction iterator methods
	void                           print();
	inline iterator                begin()       { return InstList.begin(); }
	inline const_iterator		   begin() const { return InstList.begin(); }
	inline iterator                end  ()       { return InstList.end();   }
	inline const_iterator		   end	() const { return InstList.end();   }
	inline size_t                  size ()       { return InstList.size();  }
	inline bool                    empty()       { return InstList.empty(); }
	inline       Instruction      &front()       { return InstList.front(); }
	inline       Instruction      &back ()       { return InstList.back();  }

	// RTTI
	static bool classof(const Value *V) {
		return V->getValueID() == Value::BasicBlockVal;
	}

	InstListType InstList;
};


















}