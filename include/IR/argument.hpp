#pragma once
#include "common/common.hpp"
#include "IR/Value.hpp"


namespace IR {

struct Function;

/// This class represents an incoming formal argument to a Function. A formal
/// argument, since it is ``formal'', does not contain an actual value but
/// instead represents the type, argument number, and attributes of an argument
/// for a specific function. When used in the body of said function, the
/// argument of course represents the value of the actual argument that the
/// function was called with.
struct Argument : public Value {
  struct Function *parent;
  unsigned ArgNo;

  void setParent(struct Function *parent) { this->parent = parent; };

public:
  /// Argument constructor.
  Argument(Type *Ty, const std::string &Name = "", struct Function *F = nullptr,
                    unsigned ArgNo = 0);

  inline const struct Function *getParent() const { return parent; }
  inline       struct Function *getParent()       { return parent; }

  /// Return the index of this formal argument in its containing function.
  ///
  /// For example in "void foo(int a, float b)" a is 0 and b is 1.
  unsigned getArgNo() const {
    return ArgNo;
  }

  static bool classof(const Value *V) {
    return V->getValueID() == ArgumentVal;
  }
};




}