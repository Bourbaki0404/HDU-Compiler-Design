#ifndef __LEXER_H
#define __LEXER_H

#include "lexer/token.hpp"



std::string token_serialize(const token &token);
std::vector<token> tokenize(const std::string &code);
std::string readSrc(std::string path);
void printTokens(const std::vector<token> &tokens);

#endif

