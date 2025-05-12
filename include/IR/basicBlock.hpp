#pragma once
#include "common/common.hpp"
#include "common/dlist.hpp"
#include "IR/instruction.hpp"
#include "IR/constant.hpp"
#include "IR/asmWriter.hpp"

namespace IR{

struct Function;

struct BasicBlock : public Value {
	BasicBlock() { parent = nullptr; }
	BasicBlock(const BasicBlock &) = delete;
	BasicBlock &operator=(const BasicBlock &) = delete;
	std::string label;
	struct Function *parent;
	using InstListTy = dlist<Instruction>;

	//
	using iterator = InstListTy::iterator;
	/// Instruction iterator methods
	void                           print();
	inline iterator                begin()       { return InstList.begin(); }
	inline iterator                end  ()       { return InstList.end();   }

	inline size_t                   size()       { return InstList.size();  }
	inline bool                    empty()       { return InstList.empty(); }
	inline       Instruction      *front()       { return InstList.front(); }
	inline       Instruction       *back()       { return InstList.back();  }

	// RTTI
	static bool classof(const Value *V) {
		return V->getValueID() == Value::BasicBlockVal;
	}

	InstListTy InstList;
};


















}