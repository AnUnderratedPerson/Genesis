#include "./TokenClass.hpp"

struct LexerException {
    std::string message;
    int line = 1;

    std::string what() { return message; }
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

    void advanceCurrent() {
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
            advanceCurrent();
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
        advanceCurrent();
        std::string str;

        if (at() == '"') {
            tokens.push_back(TokenInstance{TokenClass::T_STRING, str});
            return;
        }

        while (!atEnd()) {
            char _char = at();

            if (_char == '"')
                break;

            str += at();
            advanceCurrent();
        }

        if (atEnd())
            throw LexerException{"Unterminated string literal...", line};

        tokens.push_back(TokenInstance {TokenClass::T_STRING, str});
    }

    void parseNumber() {
        std::string num;

        while (!atEnd() && isDigit(at())) {
            num += at();
            advanceCurrent();
        }

        if ((at() == 'e') || (at() == '.')){
            num += at();
            advanceCurrent();
        }

        while (!atEnd() && isDigit(at())) {
            num += at();
            advanceCurrent();
        }

        current--;
        tokens.push_back(TokenInstance {TokenClass::T_NUMBER, num});
    }

    void parseComment() {
        advanceCurrent();
        while (!atEnd() && at() != '\n') {
            advanceCurrent();
        }

        current--;
    }

    void parseIdentifier() {
        std::string id;

        while (!atEnd() && isAlphaNumeric(at())) {
            id += at();
            advanceCurrent();
        }

        current--;
        if (tokenClasses.find(id) != tokenClasses.end())
            tokens.push_back(TokenInstance {tokenClasses[id], id});
        else
            tokens.push_back(TokenInstance {TokenClass::T_IDENTIFIER, id});
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
                if (isDigit(_current))
                    parseNumber();
                else if (isAlpha(_current))
                    parseIdentifier();
                else
                    throw LexerException{format("Unknown character '%c' found while reading file...", {_current}), line};
                    
                break;
            }

            advanceCurrent();
        }

        return tokens;
    }
};