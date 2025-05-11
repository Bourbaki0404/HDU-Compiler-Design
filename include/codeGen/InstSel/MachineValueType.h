
#include "common/common.hpp"

/// Machine Value Type. Every type that is supported natively by some
/// processor targeted by LLVM occurs here. This means that any legal value
/// type can be represented by an MVT.
struct MVT {
public:
    enum SimpleValueType : uint8_t {
        // Simple value types that aren't explicitly part of this enumeration
        // are considered extended value types.
        INVALID_SIMPLE_VALUE_TYPE = 0,

        // If you change this numbering, you must change the values in
        // ValueTypes.td as well!
        Other          =   1,   // This is a non-standard value
        i1             =   2,   // This is a 1 bit integer value
        i8             =   3,   // This is an 8 bit integer value
        i16            =   4,   // This is a 16 bit integer value
        i32            =   5,   // This is a 32 bit integer value
        i64            =   6,   // This is a 64 bit integer value
        i128           =   7,   // This is a 128 bit integer value

        FIRST_INTEGER_VALUETYPE = i1,
        LAST_INTEGER_VALUETYPE  = i128,

        bf16           =   8,   // This is a 16 bit brain floating point value
        f16            =   9,   // This is a 16 bit floating point value
        f32            =  10,   // This is a 32 bit floating point value
        f64            =  11,   // This is a 64 bit floating point value
        f80            =  12,   // This is a 80 bit floating point value
        f128           =  13,   // This is a 128 bit floating point value
        ppcf128        =  14,   // This is a PPC 128-bit floating point value

        FIRST_FP_VALUETYPE = bf16,
        LAST_FP_VALUETYPE  = ppcf128
    };

    SimpleValueType SimpleTy = INVALID_SIMPLE_VALUE_TYPE;

    MVT() = default;
    MVT(SimpleValueType SVT) : SimpleTy(SVT) {}
        static MVT getIntegerVT(unsigned BitWidth) {
      switch (BitWidth) {
      default:
        return (MVT::SimpleValueType)(MVT::INVALID_SIMPLE_VALUE_TYPE);
      case 1:
        return MVT::i1;
      case 8:
        return MVT::i8;
      case 16:
        return MVT::i16;
      case 32:
        return MVT::i32;
      case 64:
        return MVT::i64;
      case 128:
        return MVT::i128;
      }
    }

    static MVT getFloatingPointVT(unsigned BitWidth) {
      switch (BitWidth) {
      default:
        std::cout << "invalid bit size in MVT\n";
        exit(1);
      case 16:
        return MVT::f16;
      case 32:
        return MVT::f32;
      case 64:
        return MVT::f64;
      case 80:
        return MVT::f80;
      case 128:
        return MVT::f128;
      }
    }
};
