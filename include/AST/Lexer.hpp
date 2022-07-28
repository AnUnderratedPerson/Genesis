#include "./TokenClass.hpp"

class LexerException : public Exception {
public:
    std::string message;

    LexerException(std::string message) : message(message) {}

    std::string exception() {
        return message;
    }
};

class Lexer {
private:
    std::string sourceCode;
    std::vector<TokenInstance> tokens;
    int current = 0, line = 1;

    std::unordered_map<std::string, TokenClass> tokenClasses = {
        { "let", TokenClass::T_LET },
        { "function", TokenClass::T_FUNCTION },
        { "if", TokenClass::T_IF },
        { "else", TokenClass::T_ELSE },
        { "elseif", TokenClass::T_ELSEIF },
        { "class", TokenClass::T_CLASS },
        { "switch", TokenClass::T_SWITCH },
        { "case", TokenClass::T_CASE },
        { "default", TokenClass::T_DEFAULT },
        { "for", TokenClass::T_FOR },
        { "while", TokenClass::T_WHILE },
        { "return", TokenClass::T_RETURN },
        { "break", TokenClass::T_BREAK },
        { "continue", TokenClass::T_CONTINUE },
        { "true", TokenClass::T_TRUE },
        { "false", TokenClass::T_FALSE },
        { "null", TokenClass::T_NULL },
    };

public:
    Lexer(std::string source) : sourceCode(source) {}

    bool atEnd() {
        return (current >= sourceCode.size());
    }

    void advance() {
        current++;
    }

    char at() {
        return sourceCode[current];
    }

    char seek() {
        return ((current + 1) > sourceCode.size() ? sourceCode[current + 1] : '\0');
    }

    bool next(char _next) {
        if (preview(_next)) {
            advance();
            return true;
        }

        return false;
    }

    bool preview(char _preview) {
        int _at = current + 1;

        if (_at >= sourceCode.length())
            return false;

        return (sourceCode[_at] == _preview);
    }

    bool previous(char _previous) {
        int _at = current - 1;

        if (_at < 0)
            return false;

        return (sourceCode[_at] == _previous);
    }

    bool isDigit(char input) {
        return (input >= '0' && input <= '9');
    }

    bool isAlpha(char input) {
        return ((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z') || input == '_');
    }

    bool isAlphaNumeric(char input) {
        return (isAlpha(input) || isDigit(input));
    }

    void parseString() {
        advance();
        std::string constructor;

        if (at() == '"') {
            tokens.push_back(TokenInstance{TokenClass::T_STRING, constructor});
            return;
        }

        while (!atEnd()) {
            char _char = at();

            if (_char == '"')
                break;

            constructor += at();
            advance();
        }

        if (atEnd()) {
            std::stringstream str;

            str << "LexerException:\n" 
                    << "  Line: " << line << "\n"
                    << "  Unterminated string literal found:\n  \"" << constructor << "\n";

            throw LexerException(str.str());
        }

        tokens.push_back(TokenInstance{TokenClass::T_STRING, constructor});
    }

    void parseNumber() {
        std::string constructor;

        while (!atEnd() && isDigit(at())) {
            constructor += at();
            advance();
        }

        if ((at() == 'e') || (at() == '.')){
            constructor += at();
            advance();
        }

        while (!atEnd() && isDigit(at())) {
            constructor += at();
            advance();
        }

        current--;
        tokens.push_back(TokenInstance{TokenClass::T_NUMBER, constructor});
    }

    void parseComment() {
        advance();

        while (!atEnd() && at() != '\n') {
            advance();
        }

        current--;
    }

    void parseIdentifier() {
        std::string constructor;

        while (!atEnd() && isAlphaNumeric(at())) {
            constructor += at();
            advance();
        }

        current--;
        if (tokenClasses.find(constructor) != tokenClasses.end())
            tokens.push_back( {tokenClasses[constructor], constructor});
        else
            tokens.push_back({TokenClass::T_IDENTIFIER, constructor});
    }

    std::vector<TokenInstance> compile() {
        while (!atEnd()) {
            char _current = sourceCode.at(current);

            switch (_current) {
            case '(':
                tokens.push_back({TokenClass::T_LEFTPAREN, "("});
                break;
            case ')':
                tokens.push_back( {TokenClass::T_RIGHTPAREN, "("});
                break;
            case '{':
                tokens.push_back( {TokenClass::T_LEFTBRACE, "}"});
                break;
            case '}':
                tokens.push_back( {TokenClass::T_RIGHTBRACE, "}"});
                break;
            case '[':
                tokens.push_back( {TokenClass::T_LEFTBRACK, "["});
                break;
            case ']':
                tokens.push_back( {TokenClass::T_RIGHTBRACK, "]"});
                break;
            case ',':
                tokens.push_back( {TokenClass::T_COMMA, ","});
                break;
            case '.':
                tokens.push_back( {TokenClass::T_DOT, "."});
                break;
            case ':':
                tokens.push_back( {TokenClass::T_COLON, ":"});
                break;
            case ';':
                tokens.push_back( {TokenClass::T_SEMICOLON, ";"});
                break;
            case '\n':
                line++;
                break;
            case ' ':
            case '\t':
            case '\r':
            case '\b':
            case '\0':
                break;
            case '-':
                tokens.push_back( {TokenClass::T_MINUS, "-"});
                break;
            case '+':
                tokens.push_back( {TokenClass::T_PLUS, "+"});
                break;
            case '/':
                if (next('/'))
                    parseComment(); 
                else
                    tokens.push_back( {TokenClass::T_SLASH, "/"});
                break;
            case '*':
                tokens.push_back( {TokenClass::T_STAR, "*"});
                break;
            case '!':
                tokens.push_back( ((next('=')) ? TokenInstance{TokenClass::T_NOTEQUAL, "!="} : TokenInstance{TokenClass::T_BANG, "!"}));
                break;
            case '=':
                tokens.push_back( ((next('=')) ? TokenInstance{TokenClass::T_EQUALEQUAL, "=="} : TokenInstance{TokenClass::T_EQUAL, "="}));
                break;
            case '<':
                tokens.push_back( ((next('=')) ? TokenInstance{TokenClass::T_LESSEQUAL, "<="} : TokenInstance{TokenClass::T_LESS, "<"}));
                break;
            case '>':
                tokens.push_back( ((next('=')) ? TokenInstance{TokenClass::T_GREATEREQUAL, ">="} : TokenInstance{TokenClass::T_GREATER, ">"}));
                break;
            case '"':
                parseString();
                break;
            default:
                if (isDigit(_current)) {
                    parseNumber();
                }
                else if (isAlpha(_current)) {
                    parseIdentifier();
                }
                else {
                    std::stringstream str;

                    str << "Genesis | LexerException:\n"
                        << "  Line: " << line << "\n"
                        << "  Unknown character found while parsing file:\n  >'" << _current << "'\n";

                    throw LexerException(str.str());
                }                    
                break;
            }

            advance();
        }

        return tokens;
    }
};