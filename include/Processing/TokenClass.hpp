#ifndef GENESIS_IMPORT_NAMESPACE
#define GENESIS_IMPORT_NAMESPACE

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

struct CoreException {
    std::string message;

    void what() {
        std::cerr << "[Core Exception]: >> " << message << std::endl;
    }
};

#endif

enum class TokenClass {
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_LBRACKET,
    T_RBRACKET,
    T_SEMICOLON,
    T_COMMA,
    T_DOT,
    T_COLON,
    T_ATSIGN,

    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_EQUALEQUAL,
    T_EQUAL,
    T_NOTEQUAL,
    T_LESSEQUAL,
    T_GREATEREQUAL,
    T_LESS,
    T_GREATER,

    T_AND,
    T_OR,
    T_NOT,
    T_IF,
    T_ELSE,
    T_ELSEIF,
    T_ENDIF,
    T_WHILE,
    T_RETURN,
    T_TRUE,
    T_FALSE,
    T_NULL,
    T_FUNCTION,
    T_DEFINEPROCESSOR,

    T_IDENTIFIER,
    T_NUMBER_LITERAL,
    T_STRING_LITERAL,
};

struct TokenInstance {
    TokenClass tokenClass;
    std::string tokenValue;
    int line;
};