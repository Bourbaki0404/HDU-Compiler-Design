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
    IR::BasicBlock *BB = new IR::BasicBlock;
    auto intTy = IR::Type::getInt32Ty();
    builder->setInsertPoint(BB);
    auto intconst = IR::Constant::getIntegerValue(intTy, 2);
    auto v1 = builder->CreateAdd(intTy, intconst, intconst);
    auto v2 = builder->CreateSub(intTy, v1, intconst);
    auto v3 = builder->CreateSDiv(intTy, v2, intconst);
    auto v4 = builder->CreateUDiv(intTy, v3, intconst);
    auto curBB = builder->getInsertBlock();

    IR::BasicBlock *BB1 = new IR::BasicBlock;
        builder->setInsertPoint(BB1);
        IR::Function *F = new IR::Function(IR::FunctionType::get(IR::Type::getVoidTy(), {intTy, intTy}, false), IR::Function::ExternalLinkage, "test", mod);
    auto v5 = builder->CreateAdd(intTy, intconst, intconst);
    auto v6 = builder->CreateSub(intTy, v5, intconst);
    auto v7 = builder->CreateSDiv(intTy, v6, intconst);
    auto v8 = builder->CreateUDiv(intTy, v7, intconst);
    // IR::Function *T = new IR::Function(nullptr, mod);
    F->getBasicBlockList().push_back(BB);
    F->getBasicBlockList().push_back(BB1);
    F->print();

    v5->setName("v5");
    F->print();
    // builder->
    return 0;
}