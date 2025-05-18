#pragma once



namespace IR {

struct Function;
struct BasicBlock;

struct Verifier {
    /// Verify the function
    bool verify(Function &F);

    /// Verify the basic block
    bool verify(BasicBlock &BB);
};

} // end namespace IR