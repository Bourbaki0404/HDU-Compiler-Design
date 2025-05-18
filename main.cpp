#include<iostream>
#include<algorithm>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "types/TypeChecker.hpp"
#include "codeGen/codeGen.hpp"

#include "IR/Value.hpp"
#include "IR/basicBlock.hpp"
#include "IR/instruction.hpp"
#include "IR/IRBuilder.hpp"
#include "IR/constant.hpp"
#include "IR/module.hpp"
#include "IR/asmWriter.hpp"
#include "IR/CFG.hpp"
#include "common/Graph.hpp"
#include "Analysis/DominatorTree.hpp"
#include "Transform/Mem2Reg.hpp"


// #define GenerateParser

int (main) (int argc, char* argv[]) {
//     assert(argc == 2);
//     auto info = tokenize(readSrc(std::string(argv[1]))); // after initializing tc with rows, it will become no longer valid.
//     const auto &tokens = info.tokens;
// //     printTokens(tokens);
// // //     // printAllRules(parserRules);
// // #ifdef GenerateParser
// //     const auto &firstSets = computeFirstSet(parserRules);
// // //     // printFirstOrFollowSets(firstSets, true);
// // //     // const auto &followSets = computeFollowSet(parserRules, firstSets);
// // //     // printFirstOrFollowSets(followSets, false);
// //     lrState start(computeClosure({lrItem(parserRules[0], 0, END_OF_FILE)}, firstSets));
// // //     // printLR1Items(computeNextState(start, ID, firstSets).items);
// //     const auto &table = computeLRTable(firstSets, start);
// //     exportLRTableToCSV(table, "table.csv");
// // #endif
//     LRTable table1 = importLRTableFromCSV("table.csv");
//     auto result = std::move(Parse(tokens, table1, true));

//     if(result.node == nullptr) {
//         return 1;
//     }
//     result.node->ptr->printAST("", "");

//     TypeChecker *ptr = new TypeChecker;
//     ptr->setSource(std::move(info.rows));// move assignment
//     ptr->analyze(dynamic_cast<program*>(result.node->ptr.get()));
//     result.node->ptr.get()->printAST("", "");
//     ptr->dumpErrors(std::string(argv[1]));
//     if(ptr->hasTypeError()) {
//         // std::cout << "\nThe program has semantics error, thus compilation stops.\n";
//         return 1;
//     }
//     codeGen codegen;
//     struct program *program = dynamic_cast<struct program*>(result.node->ptr.get());
//     codegen.analyze(program);

//     TypeFactory::deleteAll();
    IR::Module *mod = new IR::Module;
    IR::IRBuilder *builder = new IR::IRBuilder;

    std::cout << "hello" << std::endl;
    
    // Create function
    auto intTy = IR::Type::getInt32Ty();
    IR::Function *F = new IR::Function(IR::FunctionType::get(IR::Type::getVoidTy(), {intTy, intTy}, false), 
                                      IR::Function::ExternalLinkage, "test", mod);

    std::cout << "hello" << std::endl;
    
    // Create basic blocks for nested conditionals
    IR::BasicBlock *entryBB = new IR::BasicBlock();
    IR::BasicBlock *thenBB1 = new IR::BasicBlock();
    IR::BasicBlock *elseBB1 = new IR::BasicBlock();
    IR::BasicBlock *thenBB2 = new IR::BasicBlock();
    IR::BasicBlock *elseBB2 = new IR::BasicBlock();
    IR::BasicBlock *mergeBB = new IR::BasicBlock();

        std::cout << "hello" << std::endl;
    
    // Add blocks to function
    F->getBasicBlockList().push_back(entryBB);
    F->getBasicBlockList().push_back(thenBB1);
    F->getBasicBlockList().push_back(elseBB1);
    F->getBasicBlockList().push_back(thenBB2);
    F->getBasicBlockList().push_back(elseBB2);
    F->getBasicBlockList().push_back(mergeBB);
    
    // Entry block
    builder->setInsertPoint(entryBB);

    /// insert alloca
    IR::AllocaInst *AI = builder->CreateAlloca(intTy, "AI");
    IR::AllocaInst *AI2 = builder->CreateAlloca(intTy, "AI2");
    IR::AllocaInst *AI3 = builder->CreateAlloca(intTy, "AI3");

    auto intconst = IR::Constant::getIntegerValue(intTy, 2);
    auto intconst2 = IR::Constant::getIntegerValue(intTy, 3);
    auto intconst3 = IR::Constant::getIntegerValue(intTy, 4);
    builder->CreateStore(intconst, AI);
    builder->CreateStore(intconst2, AI2);
    builder->CreateStore(intconst3, AI3);
    IR::LoadInst *LI = builder->CreateLoad(intTy, AI, "LI");
    IR::LoadInst *LI2 = builder->CreateLoad(intTy, AI2, "LI2");
    IR::LoadInst *LI3 = builder->CreateLoad(intTy, AI3, "LI3");
    builder->CreateAdd(intTy, LI, LI2, "add");

    auto v1 = builder->CreateAdd(intTy, intconst, intconst, "v1");
    auto cond1 = builder->CreateICmpEQ(v1, intconst, "cond1");
    builder->CreateCondBr(cond1, thenBB1, elseBB1);
    
    // Then block 1 (outer if)
    builder->setInsertPoint(thenBB1);
    auto v2 = builder->CreateAdd(intTy, intconst, intconst, "v2");
    auto cond2 = builder->CreateICmpEQ(v2, intconst, "cond2");
    builder->CreateCondBr(cond2, thenBB2, elseBB2);


    // Then block 2 (inner if)
    builder->setInsertPoint(thenBB2);
    auto v3 = builder->CreateAdd(intTy, intconst, intconst, "v3");
    builder->CreateBr(mergeBB);
    
    // Else block 2 (inner else)
    builder->setInsertPoint(elseBB2);
    auto v4 = builder->CreateSub(intTy, intconst, intconst, "v4");
    builder->CreateBr(mergeBB);
    
    // Else block 1 (outer else)
    builder->setInsertPoint(elseBB1);
    auto v5 = builder->CreateMul(intTy, intconst, intconst, "v5");
    builder->CreateBr(mergeBB);
    
    // Merge block
    builder->setInsertPoint(mergeBB);
    builder->CreateRetVoid();
    
    // Print the function

    F->print();

    // IR::DominatorTreeWrapper *DT = new IR::DominatorTreeWrapper();
    // DT->runOnFunction(*F);
    // // DT->getAnalysisResult()->dump();

    PassManager *PM = new PassManager();
    IR::Mem2Reg *M2R = new IR::Mem2Reg();
    M2R->setPassManager(PM);
    M2R->runOnFunction(*F);
    F->print();
    return 0;
}