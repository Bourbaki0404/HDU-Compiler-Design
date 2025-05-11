
#include "codeGen/InstSel/MachineValueType.h"
#include "codeGen/codeGen.hpp"

struct EVT {
  public:
    MVT V = MVT::INVALID_SIMPLE_VALUE_TYPE;
    llvm::Type *LLVMTy = nullptr;

  public:
    EVT() = default;
    EVT(MVT::SimpleValueType SVT) : V(SVT) {}
    EVT(MVT S) : V(S) {}

    bool operator==(EVT VT) const {
      return !(*this != VT);
    }
    bool operator!=(EVT VT) const {
      if (V.SimpleTy != VT.V.SimpleTy)
        return true;
      if (V.SimpleTy == MVT::INVALID_SIMPLE_VALUE_TYPE)
        return LLVMTy != VT.LLVMTy;
      return false;
    }

    /// Returns the EVT that represents a floating-point type with the given
    /// number of bits. There are two floating-point types with 128 bits - this
    /// returns f128 rather than ppcf128.
    static EVT getFloatingPointVT(unsigned BitWidth) {
      return MVT::getFloatingPointVT(BitWidth);
    }

    /// Returns the EVT that represents an integer with the given number of
    /// bits.
    static EVT getIntegerVT(llvm::LLVMContext &Context, unsigned BitWidth) {
      MVT M = MVT::getIntegerVT(BitWidth);
      if (M.SimpleTy != MVT::INVALID_SIMPLE_VALUE_TYPE)
        return M;
      return getExtendedIntegerVT(Context, BitWidth);
    }

    static EVT EVT::getExtendedIntegerVT(llvm::LLVMContext &Context, unsigned BitWidth) {
        EVT VT;
        VT.LLVMTy = llvm::IntegerType::get(Context, BitWidth);
        assert(VT.isExtended() && "Type is not extended!");
        return VT;
    }

        /// Test if the given EVT is simple (as opposed to being extended).
    bool isSimple() const {
      return V.SimpleTy != MVT::INVALID_SIMPLE_VALUE_TYPE;
    }

    /// Test if the given EVT is extended (as opposed to being simple).
    bool isExtended() const {
      return !isSimple();
    }
};