#include "lexer/lexer.hpp"

std::string readSrc(std::string path) {
    std::ifstream srcfile(path);
    if(!srcfile.is_open()) {
        std::cout << "Error: Could not open file " << path << "'\n";
    }
    return std::move(std::string(
        (std::istreambuf_iterator<char>(srcfile)),
        std::istreambuf_iterator<char>()
    ));
}


size_t read_id_keyword(const std::string &code, size_t start) {
    size_t i = start;
    std::string str;
    while(isalpha(code[i]) || isdigit(code[i]) || code[i] == '_') {
        str += code[i];
        i++;
    }
    return i;
}

size_t read_num(const std::string &code, size_t start) {
    size_t i = start;
    std::string str;
    if(code[i] == '0' && (code[i+1] == 'x' || code[i+1] == 'X')) {
        i+=2;
        while(i < code.size() && (isdigit(code[i]) || (tolower(code[i]) >= 'a' && tolower(code[i]) <= 'f')))
            i++;
    } else {
        while(isdigit(code[i]) && i < code.size()) i++;
    }
    return i;
}

const std::map<std::string, token::tokenType> keywords = {
    {"if", token::KW_IF},
    {"else", token::KW_ELSE},
    {"while", token::KW_WHILE},
    {"for", token::KW_FOR},
    {"return", token::KW_RETURN},
    {"function", token::KW_FUNCTION},
    {"break", token::KW_BREAK},
    {"continue", token::KW_CONTINUE},

    //Class
    {"class", token::KW_CLASS},
    {"struct", token::KW_STRUCT},

    //Types
    {"int", token::KW_INT},
    {"void", token::KW_VOID},
    {"float", token::KW_FLOAT},
    {"char", token::KW_CHAR},
    {"const", token::KW_CONST}
};

const std::map<std::string, token::tokenType> separators = {
    {"(", token::tokenType::LPR},
    {")", token::tokenType::RPR},
    {"[", token::tokenType::LBK},
    {"]", token::tokenType::RBK},
    {"{", token::tokenType::LBC},
    {"}", token::tokenType::RBC},
    {",", token::tokenType::COMMA},
    {";", token::tokenType::SEMICOLON}
};

const std::map<std::string, token::tokenType> operators = {
    // Arithmetic
    {"+", token::PLUS}, {"-", token::MINUS},
    {"*", token::STAR}, {"/", token::SLASH},
    {"%", token::PERCENT},
    {"++", token::PLUS_PLUS}, {"--", token::MINUS_MINUS},
    {"+=", token::PLUS_EQ}, {"-=", token::MINUS_EQ},
    {"*=", token::STAR_EQ}, {"/=", token::SLASH_EQ},
    {"%=", token::PERCENT_EQ},

    // Comparison
    {"==", token::EQ}, {"!=", token::NEQ},
    {"<", token::LT}, {">", token::GT},
    {"<=", token::LTE}, {">=", token::GTE},

    // Logical
    {"&&", token::AND}, {"||", token::OR},
    {"!", token::NOT},

    // Bitwise
    {"&", token::BIT_AND}, {"|", token::BIT_OR},
    {"^", token::BIT_XOR},
    {"<<", token::SHL}, {">>", token::SHR},

    // Assign
    {"=", token::tokenType::ASSIGN},

    // Member Access
    {".", token::tokenType::DOT},
    {"->", token::tokenType::POINTER_ACC}
};

size_t read_operator(const std::string &code, size_t start) {
    if(operators.count(code.substr(start, 1))) {
        if(operators.count(code.substr(start, 2))) {
            return start + 2; 
        }
        return start + 1;
    }
    return start;
}

bool is_keyword(std::string str) {
    std::cout << str << keywords.count(str) << "\n";
    return keywords.count(str) > 0;
}

std::string token_serialize(const token& token) {
    static const std::map<token::tokenType, std::string> type_names = {
        // Identifiers and literals
        {token::ID, "ID"}, {token::NUM, "NUM"}, {token::STRING, "STRING"},

        // Brackets and delimiters
        {token::LPR, "LPR"}, {token::RPR, "RPR"},
        {token::LBK, "LBK"}, {token::RBK, "RBK"},
        {token::LBC, "LBC"}, {token::RBC, "RBC"},
        {token::COMMA, "COMMA"}, {token::SEMICOLON, "SEMICOLON"},
        {token::COLON, "COLON"}, {token::DOT, "DOT"},

        // Operators
        {token::ASSIGN, "ASSIGN"},
        {token::PLUS, "PLUS"}, {token::MINUS, "MINUS"},
        {token::STAR, "STAR"}, {token::SLASH, "SLASH"},
        {token::PERCENT, "PERCENT"},
        {token::PLUS_PLUS, "++"}, {token::MINUS_MINUS, "--"},
        {token::PLUS_EQ, "+="}, {token::MINUS_EQ, "-="},
        {token::STAR_EQ, "*="}, {token::SLASH_EQ, "/="},
        {token::PERCENT_EQ, "%="},
        {token::EQ, "=="}, {token::NEQ, "!="},
        {token::LT, "<"}, {token::GT, ">"},
        {token::LTE, "<="}, {token::GTE, ">="},
        {token::AND, "&&"}, {token::OR, "||"},
        {token::NOT, "!"},
        {token::BIT_AND, "&"}, {token::BIT_OR, "|"},
        {token::BIT_XOR, "^"},
        {token::SHL, "<<"}, {token::SHR, ">>"}, {token::POINTER_ACC, "->"},

        // Keywords
        {token::KW_IF, "IF"}, {token::KW_ELSE, "ELSE"},
        {token::KW_WHILE, "WHILE"}, {token::KW_FOR, "FOR"},
        {token::KW_RETURN, "RETURN"}, {token::KW_FUNCTION, "FUNCTION"},
        {token::KW_CLASS, "CLASS"}, {token::KW_STRUCT, "STRUCT"},
        {token::KW_INT, "INT"}, {token::KW_VOID, "VOID"},
        {token::KW_FLOAT, "FLOAT"}, {token::KW_CHAR, "CHAR"},
        {token::KW_CONST, "CONST"}, {token::KW_BREAK, "BREAK"}, {token::KW_CONTINUE, "CONTINUE"},

        // Special
        {token::END_OF_FILE, "EOF"}, {token::INVALID, "INVALID"}
    };

    std::string type_str;
    if(type_names.find(token.type) != type_names.end()) {
        type_str = type_names.at(token.type);
    } else {
        std::cout << "token not legal\n";
        exit(1);
    }
    auto [line, col] = token.location;
    std::ostringstream oss;
    oss << std::setw(10) << std::left << type_str 
        << " '" << std::setw(10) << std::left << token.value << "'"  // Fixed width for value
        << " ( " << std::setw(2) << line << ", " 
        << std::setw(2) << col << ")\n";
    return oss.str();
}


std::vector<token> tokenize(const std::string &code) {
    size_t row = 1, col = 1, i = 0;
    std::vector<token> tokens;
    
    while(i != code.size()) {
        char cur = code[i];
        if(code.substr(i, 2) == "/*") {
            i += 2;
            while(code.substr(i, 2) != "*/" && i < code.size()) i++;
            i += 2;
        } else if(cur == ' ') {
            i++;
            col++;
        } else if(cur == '\n') {
            i++;
            row++;
            col = 1;
        } else if(isalpha(cur) || cur == '_'){
            size_t end = read_id_keyword(code, i);
            std::string str = code.substr(i, end - i);
            if(is_keyword(str)) {
                auto tokenType = keywords.at(str);
                tokens.emplace_back(tokenType, str, row, col);
            } else {
                tokens.emplace_back(token::tokenType::ID, str, row, col);
            } 
            col += end - i;
            i = end;
        } else if(isdigit(cur)){
            size_t end = read_num(code, i);
            tokens.emplace_back(token::tokenType::NUM, code.substr(i, end - i), row, col);
            col += end - i;
            i = end;
        } else if(separators.find(std::string(1, cur)) != separators.end()) {
            tokens.emplace_back(separators.at(std::string(1, cur)), std::string(1, code[i]), row, col);
            col++;
            i++;
        } else if(code.substr(i, 2) == "//") {
            while(code[i] != '\n' && i < code.size()) {
                i++;
                col++;
            } 
        } else if(read_operator(code, i) > i) {
            size_t end = read_operator(code, i);
            tokens.emplace_back(operators.at(code.substr(i, end - i)), code.substr(i, end - i), row, col);
            col += end - i;
            i = end;
        } else {
            // std::cout << "row: " << row << "col: " << col << " of " << code.substr(i, 2) << std::endl;
            i++;
        }
    }
    return tokens;
}

void printTokens(const std::vector<token> &tokens) {
    std::cout << "Tokens of The Source Program\n";
    std::cout << "-----------------------------\n";
    for(size_t i = 0; i < tokens.size(); i++) {
        std::cout << i << ") " << token_serialize(tokens[i]);
    }
    std::cout << "-----------------------------\n";
    std::cout << "Count " << tokens.size() << " tokens \n";
}