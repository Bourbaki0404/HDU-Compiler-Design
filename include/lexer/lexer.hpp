#ifndef __LEXER_H
#define __LEXER_H

#include "lexer/token.hpp"

struct lexInfo
{
    std::vector<token> tokens;
    std::vector<std::string> rows;
};


std::string token_serialize(const token &token);
lexInfo tokenize(const std::string &code);
std::string readSrc(std::string path);
void printTokens(const std::vector<token> &tokens);

#endif

