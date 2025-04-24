#include<iostream>
#include<algorithm>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "types/TypeChecker.hpp"
#include "codeGen/codeGen.hpp"

// #define GenerateParser

int (main) (int argc, char* argv[]) {
    assert(argc == 2);
    const std::vector<token> &tokens = tokenize(readSrc(std::string(argv[1])));

    printTokens(tokens);
//     // printAllRules(parserRules);
#ifdef GenerateParser
    const auto &firstSets = computeFirstSet(parserRules);
//     // printFirstOrFollowSets(firstSets, true);
//     // const auto &followSets = computeFollowSet(parserRules, firstSets);
//     // printFirstOrFollowSets(followSets, false);
    lrState start(computeClosure({lrItem(parserRules[0], 0, END_OF_FILE)}, firstSets));
//     // printLR1Items(computeNextState(start, ID, firstSets).items);
    const auto &table = computeLRTable(firstSets, start);
    exportLRTableToCSV(table, "table.csv");
#endif
    LRTable table1 = importLRTableFromCSV("table.csv");
    auto result = std::move(Parse(tokens, table1, true));

    if(result.node == nullptr) {
        return 1;
    }
    result.node->ptr->printAST("", "");

    TypeChecker *ptr = new TypeChecker;
    ptr->analyze(dynamic_cast<program*>(result.node->ptr.get()));
    result.node->ptr.get()->printAST("", "");
    ptr->dumpErrors(std::string(argv[1]));
    // if(ptr->hasTypeError()) {
    //     // std::cout << "\nThe program has semantics error, thus compilation stops.\n";
    //     return 1;
    // }
    // codeGen codegen;
    // struct program *program = dynamic_cast<struct program*>(result.node->ptr.get());
    // codegen.analyze(program);

    TypeFactory::deleteAll();
    return 0;
}