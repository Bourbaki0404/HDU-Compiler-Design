#include<iostream>
#include<algorithm>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"


int main (int argc, char* argv[]) {
    assert(argc == 2);
    const std::vector<token> &tokens = tokenize(readSrc(std::string(argv[1])));

    printTokens(tokens);

    // printAllRules(parserRules);
    // const auto &firstSets = computeFirstSet(parserRules);
    // printFirstOrFollowSets(firstSets, true);
    // const auto &followSets = computeFollowSet(parserRules, firstSets);
    // printFirstOrFollowSets(followSets, false);
    // lrState start(computeClosure({lrItem(parserRules[0], 0, END_OF_FILE)}, firstSets));
    // printLR1Items(computeNextState(start, ID, firstSets).items);
    // const auto &table = computeLRTable(firstSets, start);
    // exportLRTableToCSV(table, "table.csv");
    LRTable table = importLRTableFromCSV("table.csv");
    auto result = std::move(Parse(tokens, table, false));
    if(result.parseTree == nullptr) {
        return -1;
    }
    // visualizeAsTree(parsetree, "", true);
    return 0;
}