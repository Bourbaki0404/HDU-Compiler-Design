#include "common/common.hpp"

namespace IR {

struct Value;

struct Module {
    Module() {}
    // std::string allocName(std::string base);
    // bool isNameConflict(const std::string &name) { return names.count(name); }
    // std::unordered_map<std::string, size_t> names;

    ValueSymbolTable *getValueSymbolTable()       { return ValSymTab; }

    // /// The type for the list of global variables.
    // using GlobalListType = SymbolTableList<GlobalVariable>;
    // /// The type for the list of functions.
    // using FunctionListType = SymbolTableList<Function>;
    // GlobalListType GlobalList;      ///< The Global Variables in the module
    // FunctionListType FunctionList;  ///< The Functions in the module
    // std::unique_ptr<ValueSymbolTable> ValSymTab; ///< Symbol table for values

    

    /// @}
    /// @name Function Iteration
    /// @{
    // iterator                begin()       { return FunctionList.begin(); }
    // const_iterator          begin() const { return FunctionList.begin(); }
    // iterator                end  ()       { return FunctionList.end();   }
    // const_iterator          end  () const { return FunctionList.end();   }
    // reverse_iterator        rbegin()      { return FunctionList.rbegin(); }
    // const_reverse_iterator  rbegin() const{ return FunctionList.rbegin(); }
    // reverse_iterator        rend()        { return FunctionList.rend(); }
    // const_reverse_iterator  rend() const  { return FunctionList.rend(); }
    // size_t                  size() const  { return FunctionList.size(); }
    // bool                    empty() const { return FunctionList.empty(); }

    ValueSymbolTable *ValSymTab;
};

}