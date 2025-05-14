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
	: Value(Type::getLabelTy(), BasicBlockVal) { parent = nullptr; }
	BasicBlock(const BasicBlock &) = delete;
	BasicBlock &operator=(const BasicBlock &) = delete;
	std::string label;
	struct Function *parent;
	using InstListType = SymbolTableList<Instruction>;

	// Get the ValueSymbolTable of the function that this basic block belongs to
	ValueSymbolTable *getValueSymbolTable();

	// helper methods for optimization
	bool isEntryBlock() const ;

	// Get the terminator instruction of the basic block, if it exists then return it, otherwise return nullptr
	const Instruction *getTerminator() const;
	Instruction	*getTerminator() ;
	
	/// Unlink 'this' from the containing function, but do not delete it.
	// 1. It first checks if the basic block has a parent function
	// 2. If it does, it retrieves the function's basic block list
	// 3. It then removes itself from that list using the remove method of the SymbolTableList
	// 4. As the setParent method is called by the SymbolTableListTraits, it will call the setSymTabObject method
	// 5. The setSymTabObject method will remove all the instruction symtab entries from the current function and reinsert them into the new function if needed
	void removeFromParent();

	/////// For SSA form optimization ///////

	/// Update all phi nodes in this basic block to refer to basic block \p New
  	/// instead of basic block \p Old.
	void replacePhiUsesWith(BasicBlock *Old, BasicBlock *New);

	/// Update all phi nodes in this basic block's successors to refer to basic
	/// block \p New instead of basic block \p Old.
	void replaceSuccessorsPhiUsesWith(BasicBlock *Old, BasicBlock *New);

	/// Update all phi nodes in this basic block's successors to refer to basic
	/// block \p New instead of to it.
	void replaceSuccessorsPhiUsesWith(BasicBlock *New);

	/// Return the underlying instruction list container.
	///
	/// Currently you need to access the underlying instruction list container
	/// directly if you want to modify it.
	const InstListType &getInstList() const { return InstList; }
	InstListType &getInstList()       { return InstList; }


	/// Returns a pointer to a member of the instruction list, used by the SymbolTableListTraits.
	static InstListType BasicBlock::*getSublistAccess(Instruction*) {
		return &BasicBlock::InstList;
	}
	
protected:
	/// The setParent() should only be called by the SymbolTableListTraits
	void 						  setParent(struct Function *parent);
public:
	inline const struct Function *getParent() const 	{ return parent; }
	inline struct Function 		 *getParent()       	{ return parent; }

	// Returns the module that this basic block belongs to
	// 
	const Module *				  getModule() const ;
	Module *					  getModule();

	using iterator = InstListType::iterator;
	using const_iterator = InstListType::const_iterator;
	using reverse_iterator = InstListType::reverse_iterator;
	using const_reverse_iterator = InstListType::const_reverse_iterator;

	/// Instruction iterator methods

	inline iterator                begin()       	{ return InstList.begin(); }
	inline const_iterator		   begin() const 	{ return InstList.begin(); }
	inline iterator                end  ()       	{ return InstList.end();   }
	inline const_iterator		   end	() const 	{ return InstList.end();   }
	inline reverse_iterator        rbegin()       	{ return InstList.rbegin(); }
	inline const_reverse_iterator  rbegin() const 	{ return InstList.rbegin(); }
	inline reverse_iterator        rend  ()       	{ return InstList.rend();   }
	inline const_reverse_iterator  rend  () const 	{ return InstList.rend();   }

	inline size_t                  size ()       	{ return InstList.size();  }
	inline bool                    empty()       	{ return InstList.empty(); }
	inline       Instruction      &front()       	{ return InstList.front(); }
	inline       Instruction      &back ()       	{ return InstList.back();  }

	// RTTI for dyn_cast and isa
	static bool classof(const Value *V) {
		return V->getValueID() == Value::BasicBlockVal;
	}

	// print the basic block
	void                           print();

	InstListType InstList;
};


















}