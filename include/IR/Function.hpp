#pragma once
#include "common/common.hpp"
#include "IR/basicBlock.hpp"
#include "IR/Value.hpp"
#include "IR/symbolTableListTraits.hpp"
#include "IR/valueSymbolTable.hpp"
#include "common/iterator_range.hpp"
#include "IR/utils.hpp"
#include "IR/Globals.hpp"

namespace IR{

struct Argument;
struct ValueSymbolTable;
struct Module;

// parent is managed by GlobalObject
struct Function : public GlobalObject, public dlist_node<Function> {
	friend class SymbolTableListTraits<Function>;
	Function(FunctionType *ty, Module *M);

	using BasicBlockListType = SymbolTableList<BasicBlock>;
	using iterator = BasicBlockListType::iterator;
	using const_iterator = BasicBlockListType::const_iterator;
	using argListType = std::vector<Argument*>;
	using arg_iterator = argListType::iterator;
	using const_arg_iterator = argListType::const_iterator;

    // void setFuncArgs(Function *Func, std::vector<std::string> FuncArgs);
    // Function *createFunc(Type *RetTy, std::vector<Type *>& Params, std::string Name, bool isVarArg = false);
    BasicBlockListType &getBasicBlockList() 	{ return basic_blocks; }
	BasicBlock		   &getEntryBlock()    	 	{ return basic_blocks.front(); }
	const BasicBlock   &getEntryBlock()	const 	{ return basic_blocks.front(); }

    /// Returns the FunctionType for me.
      
    // FunctionType *getFunctionType() const {
    //     return cast<FunctionType>(getValueType());
    // }
	static BasicBlockListType Function::*getSublistAccess(BasicBlock*) {
    	return &Function::basic_blocks;
  	}
	
    inline ValueSymbolTable *getValueSymbolTable() { return SymTab.get(); }
	inline const ValueSymbolTable *getValueSymbolTable() const {
    	return SymTab.get();
  	}

	/// Iteration of basic blocks
	iterator                begin()       { return basic_blocks.begin(); }
	const_iterator          begin() const { return basic_blocks.begin(); }
	iterator                end  ()       { return basic_blocks.end();   }
	const_iterator          end  () const { return basic_blocks.end();   }

	/// Manage the arguments
	arg_iterator arg_begin() {
		return arguments.begin();
	}

	const_arg_iterator arg_begin() const {
		return arguments.cbegin();
	}

	arg_iterator arg_end() {
		return arguments.end();
	}

	const_arg_iterator arg_end() const {
		return arguments.cend();
	}

	Argument *get_arg(size_t i) {
		__assert__(i >= 0 && i < arguments.size(), "get_arg fail");
		return arguments[i];
	}

	iterator_range<arg_iterator> args() {
		return iterator_range<arg_iterator>(arg_begin(), arg_end());
	}

	iterator_range<const_arg_iterator> args() const {
		return iterator_range<const_arg_iterator>(arg_begin(), arg_end());
	}

	size_t arg_size() { return arguments.size(); }
	bool arg_empty() { return arguments.empty(); }

	static bool classof(const Value *V) {
		return V->getValueID() == FunctionVal;
	}

private:
    BasicBlockListType basic_blocks;
    argListType arguments;
    std::unique_ptr<ValueSymbolTable> SymTab; ///< Symbol table of args/instructions
};



















}