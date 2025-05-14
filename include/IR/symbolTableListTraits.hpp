#pragma once
#include "common/common.hpp"
#include "IR/valueSymbolTable.hpp"
#include "common/dlist.hpp"

namespace IR{


struct Argument;
struct BasicBlock;
struct Function;
// class GlobalAlias;
// class GlobalIFunc;
// class GlobalVariable;
struct Module;
struct ValueSymbolTable;

/// Template metafunction to get the parent type for a symbol table list.
///
/// Implementations create a typedef called \c type so that we only need a
/// single template parameter for the list and traits.
template <typename NodeTy> struct SymbolTableListTraitParentType {};

#define DEFINE_SYMBOL_TABLE_PARENT_TYPE(NODE, PARENT)                          \
  	template <> struct SymbolTableListTraitParentType<NODE> { using type = PARENT; };
DEFINE_SYMBOL_TABLE_PARENT_TYPE(Instruction, BasicBlock)
DEFINE_SYMBOL_TABLE_PARENT_TYPE(BasicBlock, struct Function)
DEFINE_SYMBOL_TABLE_PARENT_TYPE(Argument, struct Function)
DEFINE_SYMBOL_TABLE_PARENT_TYPE(struct Function, Module)
// DEFINE_SYMBOL_TABLE_PARENT_TYPE(GlobalVariable, Module)
// DEFINE_SYMBOL_TABLE_PARENT_TYPE(GlobalAlias, Module)
// DEFINE_SYMBOL_TABLE_PARENT_TYPE(GlobalIFunc, Module)
#undef DEFINE_SYMBOL_TABLE_PARENT_TYPE




template <typename ValueSubClass>
// This class implements callback functions to automatically manage the symbol table upon the insertion or deletion of the list node.
// 1) void addNodeToList(ValueSubClass *V); 
// 2) void removeNodeFromList(ValueSubClass *V);
// 3  void transferNodesFromList(SymbolTableListTrait &L2, iterator first, iterator last);
// When node is inserted or deleted from the list, the corresponding callback function will be called and modify the associated symbol table (locals -> function symtbl or globals -> module symbtl)
// ValueSubClass   - The type of objects that I hold, e.g. Instruction.
// ItemParentClass - The type of object that owns the list, e.g. BasicBlock.
struct SymbolTableListTraits : public dlist_callback_traits<ValueSubClass> {
  	using ItemParentClass =
	typename SymbolTableListTraitParentType<ValueSubClass>::type;
	using ListTy = SymbolTableListTraits<ValueSubClass>;
public:
  SymbolTableListTraits() = default;

private:
	/// getListOwner - Return the object that owns this list.  If this is a list
	/// of instructions, it returns the BasicBlock that owns them.
	ItemParentClass *getListOwner() {
		size_t Offset = reinterpret_cast<size_t>(
			&((ItemParentClass *)nullptr->*ItemParentClass::getSublistAccess(
											static_cast<ValueSubClass *>(
												nullptr))));
		ListTy *Anchor = static_cast<ListTy *>(this);
		return reinterpret_cast<ItemParentClass*>(reinterpret_cast<char*>(Anchor)-
												Offset);
	}

	static auto &getList(ItemParentClass *Par) {
		return Par->*(Par->getSublistAccess((ValueSubClass*)nullptr));
	}

	static ValueSymbolTable *getSymTab(ItemParentClass *Par) {
		return Par ? Par->getValueSymbolTable() : nullptr;
	}

  	ItemParentClass *parent;

public:
  	void addNodeToList(ValueSubClass *V);
  	void removeNodeFromList(ValueSubClass *V);
  	// void transferNodesFromList(SymbolTableListTraits &L2, iterator first,
  	//                            iterator last);
  	template<typename TPtr>
  	void setSymTabObject(TPtr *, TPtr);
};

/// List that automatically updates parent links and symbol tables.
///
/// When nodes are inserted into and removed from this list, the associated
/// symbol table will be automatically updated.  Similarly, parent links get
/// updated automatically.
template <class T>
using SymbolTableList = dlist<T, SymbolTableListTraits<T>>;


template <typename ValueSubClass>
void SymbolTableListTraits<ValueSubClass>::addNodeToList(ValueSubClass *V) {
	std::cout << "Value inserted\n";
    if(V->getParent()) {
        std::cout << "addNodeToList Fail.\n";
        exit(1);
    }
    ItemParentClass *Owner = getListOwner();
    V->setParent(Owner);
    if (V->hasName())
        if (ValueSymbolTable *ST = getSymTab(Owner))
            ST->reinsertValue(V);
    return;
}

template <typename ValueSubClass>
void SymbolTableListTraits<ValueSubClass>::removeNodeFromList(ValueSubClass *V) {
	std::cout << "Value removed\n";
    // V->setParent(nullptr);
    // if (V->hasName())
    //     if (ValueSymbolTable *ST = getSymTab(getListOwner()))
    //         ST->removeValueName(V->getValueName());
    return;
}

/// setSymTabObject - This is called when (f.e.) the parent of a basic block
/// changes.  
/// This requires us to remove all the instruction symtab entries from
/// the current function and reinsert them into the new function.
template <typename ValueSubClass>
template <typename TPtr>
void SymbolTableListTraits<ValueSubClass>::setSymTabObject(TPtr *Dest,
                                                           TPtr Src) {
	ValueSymbolTable *OldST = getSymTab(getListOwner());

	//set parent of basic block
	*Dest = Src;

	ValueSymbolTable *NewST = getSymTab(getListOwner());

	if (OldST == NewST) return;

	auto &ItemList = getList(getListOwner());
	if (ItemList.empty()) return;

	if (OldST) {
		// Remove all entries from the previous symtab.
		for (auto I = ItemList.begin(); I != ItemList.end(); ++I)
		if (I->hasName())
			OldST->removeValueName(I->getName());
	}

	if (NewST) {
		// Add all of the items to the new symtab.
		for (auto I = ItemList.begin(); I != ItemList.end(); ++I)
		if (I->hasName())
			NewST->reinsertValue(&*I);
	}

}

}