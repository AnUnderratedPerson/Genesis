#include "TokenClass.hpp"

struct LexerException {
    std::string message;
    
    void what() {
        std::cerr << "[Lexer Exception]: >> " << message << std::endl;
    }
};

struct ProcessorItem {
    TokenClass processorClass;
    std::string processorValue;
    int tokenOffset;
};

class Lexer {
private:
    std::string source;
    std::vector<TokenInstance> tokens;
    int current, line;

    std::unordered_map<std::string, TokenClass> keywords = {
        { "if", TokenClass::T_IF },
        { "else", TokenClass::T_ELSE },
        { "elseif", TokenClass::T_ELSEIF },
        { "while", TokenClass::T_WHILE },
        { "return", TokenClass::T_RETURN },
        { "true", TokenClass::T_TRUE },
        { "false", TokenClass::T_FALSE },
        { "null", TokenClass::T_NULL },
        { "and", TokenClass::T_AND },
        { "or", TokenClass::T_OR },
        { "function", TokenClass::T_FUNCTION },
    };

    std::unordered_map<std::string, TokenClass> processors = {
        { "@define", TokenClass::T_DEFINEPROCESSOR },
        { "@if", TokenClass::T_IFPROCESSOR },
        { "@else", TokenClass::T_ELSEPROCESSOR },
        { "@elseif", TokenClass::T_ELSEIFPROCESSOR },
        { "@end", TokenClass::T_ENDPROCESSOR }
    };

    char currently() {
        return (atEnd() ? '\0' : source[current]);
    }

    bool atEnd() {
        return (current >= source.size());
    }

    bool atEnd(int location) {
        return (location >= source.size());
    }

    bool preview(char c) {
        int previewCount = current + 1;

        if (atEnd(previewCount))
            return false;

        if (source[previewCount] != c)
            return false;
            
        return true;
    }

    bool next(char c) {
        if (!preview(c))
            return false;

        advance();
        return true;
    }

    bool isADigit(char c) {
        return (c >= '0' && c <= '9');
    }

    bool isAnAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
    }

    bool isAnAlphaNumeric(char c) {
        return isAnAlpha(c) || isADigit(c);
    }

    void before() {
        current--;
    }

    void advance() {
        current++;
    }

    void createString() {
        advance();

        if (next('"')) {
            append({TokenClass::T_STRING_LITERAL, "", line});
            return;
        }

        std::string constructor; 

        while (!atEnd() && (currently() != '"')) {
            if (currently() == '\n')
                line++;
            
            constructor += currently();
            advance();
        }

        if (atEnd())
            // --> Throw a custom exception stating that the string is not unterminated.
            throw std::runtime_error("Unterminated string literal");

        append({ TokenClass::T_STRING_LITERAL, constructor, line });
    }

    void createNumber() {
        std::string constructor;

        while (!atEnd() && isADigit(currently())) {
            constructor += currently();
            advance();
        }

        if ((currently() == '.') || (currently() == 'e')) {
            constructor += currently();
            advance();
        }

        while (!atEnd() && isADigit(currently())) {
            constructor += currently();
            advance();
        }

        before();
        append({ TokenClass::T_NUMBER_LITERAL, constructor, line });
    }

    void createIdentifier() {
        std::string constructor;

        while (!atEnd() && isAnAlphaNumeric(currently())) {
            constructor += currently();
            advance();
        }

        before();
        if (keywords.find(constructor) != keywords.end())
            append({ keywords[constructor], constructor, line });
        else
            append({TokenClass::T_IDENTIFIER_LITERAL, constructor, line});
    }

    void revokeComment() {
        while (!atEnd() && (currently() != '\n'))
            advance();
    }

    void createProcessor() {
        std::string constructor;

        constructor += currently();
        advance();

        while (!atEnd() && isAnAlphaNumeric(currently())) {
            constructor += currently();
            advance();
        }
        
        if (processors.find(constructor) != processors.end()) {
            processorItems.push_back({ processors[constructor], constructor, (int) tokens.size() });
            append({ processors[constructor], constructor, line });
        }
        else {
            throw LexerException { "Invalid processor: " + constructor };
        }
    }

    void append(TokenInstance value) {
        tokens.push_back(value);
    }

public:
    std::vector<ProcessorItem> processorItems;

    Lexer(std::string source) : source(source), current(0), line(1) {}

    std::vector<TokenInstance> compile() {
        while (!atEnd()) {
            char at = source[current];

            switch (at) {
                case '(':
                    append({ TokenClass::T_LPAREN, "(", line });
                    break;
                case ')':
                    append({ TokenClass::T_RPAREN, ")", line });
                    break;
                case '{':
                    append({ TokenClass::T_LBRACE, "{", line });
                    break;
                case '}':
                    append({ TokenClass::T_RBRACE, "}", line });
                    break;
                case '[':
                    append({ TokenClass::T_LBRACKET, "[", line });
                    break;
                case ']':
                    append({ TokenClass::T_RBRACKET, "]", line });
                    break;
                case '.':
                    append({ TokenClass::T_DOT, ".", line });
                    break;
                case ',':
                    append({ TokenClass::T_COMMA, ",", line });
                    break;
                case ':':
                    append({ TokenClass::T_COLON, ":", line });
                    break;
                case ';':
                    append({ TokenClass::T_SEMICOLON, ";", line });
                    break;
                case '+':
                    append({ TokenClass::T_PLUS, "+", line });
                    break;
                case '-':
                    append({ TokenClass::T_MINUS, "-", line });
                    break;
                case '/':
                    next('/') ? revokeComment() : append({ TokenClass::T_SLASH, "/", line });
                    break;
                case '*':
                    append({ TokenClass::T_STAR, "*", line });
                    break;
                case '@':
                    createProcessor();
                    break;
                case '<':
                    next('=') ? append({ TokenClass::T_LESSEQUAL, "<=", line }) : append({ TokenClass::T_LESS, "<", line });
                    break;
                case '>':
                    next('=') ? append({ TokenClass::T_GREATEREQUAL, ">=", line }) : append({ TokenClass::T_GREATER, ">", line });
                    break;
                case '=':
                    append({ TokenClass::T_EQUAL, "=", line });
                    break;
                case '!':
                    append({ TokenClass::T_NOT, "!", line });
                    break;
                case ' ':
                case '\r':
                case '\t':
                case '\0':
                    break;
                case '"':
                    createString();
                    break;
                case '\n':
                    line++;
                    break;
                default:
                    if (isADigit(at)) {
                        createNumber();
                    }
                    else if (isAnAlpha(at)) {
                        createIdentifier();
                    }
                    else {
                        // --> Throw an exception here indicating that the character is not valid
                        throw std::runtime_error("Unknown character found while lexing...");
                    }
            }

            advance();
        }

        return tokens;
    }
};