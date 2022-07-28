#include "../Util/Util.hpp"

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
    T_EOF
};

struct TokenInstance {
    TokenClass token;
    std::string value;
    int line, column;
};

std::string tokenToString(TokenClass token) {
    std::unordered_map<TokenClass, std::string> tokenClasses = {
        {TokenClass::T_LET, "let"},
        {TokenClass::T_FUNCTION, "function"},
        {TokenClass::T_IF, "if"},
        {TokenClass::T_ELSE, "else"},
        {TokenClass::T_ELSEIF, "elseif"},
        {TokenClass::T_CLASS, "class"},
        {TokenClass::T_SWITCH, "switch"},
        {TokenClass::T_CASE, "case"},
        {TokenClass::T_DEFAULT, "default"},
        {TokenClass::T_FOR, "for"},
        {TokenClass::T_WHILE, "while"},
        {TokenClass::T_RETURN, "return"},
        {TokenClass::T_BREAK, "break"},
        {TokenClass::T_CONTINUE, "continue"},
        {TokenClass::T_TRUE, "true"},
        {TokenClass::T_FALSE, "false"},
        {TokenClass::T_NULL , "null"},

        {TokenClass::T_IDENTIFIER, "identifier"},
        {TokenClass::T_NUMBER, "number"},
        {TokenClass::T_STRING, "string"},
    };

    switch (token) {
        case TokenClass::T_LEFTPAREN:
            return "(";
        case TokenClass::T_RIGHTPAREN:
            return ")";
        case TokenClass::T_LEFTBRACE:
            return "{";
        case TokenClass::T_RIGHTBRACE:
            return "}";
        case TokenClass::T_LEFTBRACK:
            return "[";
        case TokenClass::T_RIGHTBRACK:
            return "]";
        case TokenClass::T_DOT:
            return ".";
        case TokenClass::T_COLON:
            return ":";
        case TokenClass::T_SEMICOLON:
            return ";";
        case TokenClass::T_COMMA:
            return ",";
        case TokenClass::T_PLUS:
            return "+";
        case TokenClass::T_MINUS:
            return "-";
        case TokenClass::T_SLASH:
            return "/";
        case TokenClass::T_STAR:
            return "*";
        case TokenClass::T_EQUAL:
            return "=";
        case TokenClass::T_EQUALEQUAL:
            return "==";
        case TokenClass::T_NOTEQUAL:
            return "!=";
        case TokenClass::T_GREATER:
            return ">";
        case TokenClass::T_LESS:
            return "<";
        case TokenClass::T_GREATEREQUAL:
            return ">=";
        case TokenClass::T_LESSEQUAL:
            return "<=";
        default:
            if (tokenClasses.find(token) != tokenClasses.end()) {
                return tokenClasses[token];
            } else {
                return "<none>";
            }
    }
}