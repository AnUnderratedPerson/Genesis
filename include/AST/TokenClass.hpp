#include "../Util/Source.hpp"

enum class TokenClass {
    T_LEFTPAREN,
    T_RIGHTPAREN,
    T_LEFTBRACE,
    T_RIGHTBRACE,
    T_LEFTBRACK,
    T_RIGHTBRACK,

    T_DOT,
    T_COMMA,
    T_COLON,
    T_SEMICOLON,

    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,

    T_BANG,
    T_EQUAL,

    T_EQUALEQUAL,
    T_NOTEQUAL,
    T_LESS,
    T_LESSEQUAL,
    T_GREATER,
    T_GREATEREQUAL,

    T_STRING,
    T_IDENTIFIER,
    T_NUMBER,

    T_LET,
    T_FUNCTION,
    T_IF,
    T_ELSE,
    T_ELSEIF,
    T_CLASS,
    T_SWITCH,
    T_CASE,
    T_DEFAULT,
    T_FOR,
    T_WHILE,
    T_RETURN,
    T_BREAK,
    T_CONTINUE,
    T_TRUE,
    T_FALSE,
    T_NULL,

    T_NONE,
};

struct TokenInstance {
    TokenClass token;
    std::string value;
};