#ifndef __TOKEN_H
#define __TOKEN_H

#include "common/common.hpp"

struct token {
    enum tokenType {
        // Identifiers and literals
        ID, NUM, STRING,

        // Brackets and delimiters
        LPR, RPR,        // ( )
        LBK, RBK,        // [ ]
        LBC, RBC,        // { }
        COMMA, SEMICOLON, COLON, DOT,

        // Operators
        ASSIGN,         // =
        PLUS, MINUS,    // + -
        STAR, SLASH,    // * /
        PERCENT,        // %
        PLUS_PLUS, MINUS_MINUS, // ++ --
        PLUS_EQ, MINUS_EQ,      // += -=
        STAR_EQ, SLASH_EQ,      // *= /=
        PERCENT_EQ,             // %=
        EQ, NEQ,                // == !=
        LT, GT,                 // < >
        LTE, GTE,               // <= >=
        AND, OR,                // && ||
        NOT,                    // !
        BIT_AND, BIT_OR, BIT_XOR, // & | ^
        SHL, SHR,               // << >>
        
        // Keywords
        KW_IF, KW_ELSE, KW_WHILE, KW_FOR,
        KW_RETURN, KW_FUNCTION, KW_CLASS, 

        // Type
        KW_INT, KW_VOID, KW_FLOAT, KW_STRUCT, KW_CHAR,

        // Special
        END_OF_FILE, INVALID
    };
    
    std::string value;
    tokenType type;
    std::pair<size_t, size_t> location;

    token(tokenType type, std::string value, size_t x, size_t y)
        :value(value), type(type), location({x, y}) {}
};

#endif