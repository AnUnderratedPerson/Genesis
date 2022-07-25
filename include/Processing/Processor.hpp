#include "Lexer.hpp"

struct ProcessException {
    std::string message;
    
    void what() {
        std::cerr << "[Processer Exception]: >> " << message << std::endl;
    }
};

class Processor {
private:
    std::vector<TokenInstance> tokens;
    std::vector<TokenInstance> output;
    int current;

    bool atEnd() {
        return (current >= tokens.size());
    }

    void advance() {
        current++;
    }

    void before() {
        current--;
    }

    TokenInstance currently() {
        return (atEnd() ? TokenInstance() : tokens[current]);
    }

    TokenInstance expect(std::initializer_list<TokenClass> types, std::string exception) {
        if (atEnd())
            throw ProcessException { exception };
        
        for (auto type : types) {
            if (tokens[current].tokenClass != type)
                continue;

            return tokens[current];
        }

        throw ProcessException { exception };
    }

    void define() {
        advance();

        auto name = expect({ TokenClass::T_IDENTIFIER }, "Expected an identifier for the processor but got something else instead...");
        advance();
        auto expression = expect({ 
            TokenClass::T_TRUE, TokenClass::T_FALSE, TokenClass::T_NULL 
        }, "Expected a boolean or null but got something else instead...");

        definitions[name.tokenValue] = expression;
    }

    void ifProcessor() {
        advance();

        auto expression = expect({
            TokenClass::T_TRUE, TokenClass::T_FALSE, TokenClass::T_NULL,
            TokenClass::T_IDENTIFIER
        }, "Expected a boolean, identifier or null but got something else instead...");

        advance();

        if (expression.tokenClass == TokenClass::T_IDENTIFIER) {
            // --> fetch the processor definition

            if (definitions.find(expression.tokenValue) != definitions.end())
                expression = definitions[expression.tokenValue];
            else
                expression = TokenInstance { TokenClass::T_NULL, "null", -1 };    
        }

        std::cout << expression.tokenValue << std::endl;

        switch (expression.tokenClass) {
            // --> Heres how we'll do this. 
            // --> If the processor is false, then we will just skip the rest of the code in between the if and endif processor tag
            // --> If the processor is true, then we will execute the code in between the if and endif processor tag
            // --> If the processor is false but provides an @else, then we will execute the code in between the else and endif processor tag

            case TokenClass::T_NULL:
            case TokenClass::T_FALSE:
                while (!atEnd()) {
                    if (currently().tokenClass == TokenClass::T_ATSIGN)
                        advance();

                    if ((currently().tokenClass == TokenClass::T_ELSE) || (currently().tokenClass == TokenClass::T_ENDIF))
                        break;

                    advance();
                }

                if (currently().tokenClass == TokenClass::T_ELSE) {
                    advance();

                    while (!atEnd()) {
                        if (currently().tokenClass == TokenClass::T_ATSIGN)
                            advance();

                        if (currently().tokenClass == TokenClass::T_ENDIF) {
                            advance();
                            return;
                        }

                        output.push_back(currently());
                        advance();
                    }

                    throw ProcessException { "Expected an @end processor tag but got something else instead..." };
                }

                if (currently().tokenClass != TokenClass::T_ENDIF) {
                    throw ProcessException { "Expected an @end processor tag but got something else instead..." };
                }
                break;
            case TokenClass::T_TRUE:
                while (!atEnd()) {
                    if (currently().tokenClass == TokenClass::T_ATSIGN)
                        advance();

                    if ((currently().tokenClass == TokenClass::T_ELSE) || (currently().tokenClass == TokenClass::T_ENDIF))
                        break;

                    output.push_back(currently());
                    advance();
                }

                if (currently().tokenClass == TokenClass::T_ELSE) {
                    advance();

                    while (!atEnd()) {
                        if (currently().tokenClass == TokenClass::T_ATSIGN)
                            advance();

                        if (currently().tokenClass == TokenClass::T_ENDIF) {
                            advance();
                            return;
                        }

                        advance();
                    }

                    throw ProcessException { "Expected an @end processor tag but got something else instead..." };
                }

                if (currently().tokenClass != TokenClass::T_ENDIF) {
                    throw ProcessException { "Expected an @end processor tag but got something else instead..." };
                }
                break;
        }
    }

public:
    std::unordered_map<std::string, TokenInstance> definitions;

    Processor(std::vector<TokenInstance> tokens) : tokens(tokens), output({}), current(0) {}

    std::vector<TokenInstance> process() {
        while (!atEnd()) {
            auto currentToken = currently();

            switch (currentToken.tokenClass) {
                case TokenClass::T_ATSIGN:
                    {
                        advance();
                        auto token = currently();

                        switch (token.tokenClass) {
                            case TokenClass::T_DEFINEPROCESSOR:
                                define();
                                break;
                            case TokenClass::T_IF:
                                ifProcessor();
                                break;
                            default:
                                throw ProcessException { "Expected a valid processor tag (@if or @define) but got something else instead..." };
                        }
                    }
                    break;
                default:
                    output.push_back(currently());
                    break;
            }

            advance();
        }

        return output;
    }
};