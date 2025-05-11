#include "IR/basicBlock.hpp"

namespace IR{

void BasicBlock::print() {
    AsmWriter::printBasicBlock(this);
}




}

