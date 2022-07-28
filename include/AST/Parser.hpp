#include "TokenStatement.hpp"

// Lower down = Greater precedence
enum Precedence {
    PREC_NONE,        // null
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_PRIMARY      // literals
};

class ParserException : public Exception {
public:
    std::string message;

    ParserException(std::string message) : message(message) {}

    std::string exception() {
        return message;
    }
};

class Parser {
private:
    std::vector<TokenInstance> tokens;
    std::vector<std::shared_ptr<Statement>> statements;
    std::unordered_map<TokenClass, Precedence> precedences = {
        {TokenClass::T_PLUS, Precedence::PREC_TERM},
        {TokenClass::T_MINUS, Precedence::PREC_TERM},
        {TokenClass::T_SLASH, Precedence::PREC_FACTOR},
        {TokenClass::T_STAR, Precedence::PREC_FACTOR},

        {TokenClass::T_EQUALEQUAL, Precedence::PREC_EQUALITY},
        {TokenClass::T_NOTEQUAL, Precedence::PREC_EQUALITY},

        {TokenClass::T_LESS, Precedence::PREC_COMPARISON},
        {TokenClass::T_GREATER, Precedence::PREC_COMPARISON},
        {TokenClass::T_LESSEQUAL, Precedence::PREC_COMPARISON},
        {TokenClass::T_GREATEREQUAL, Precedence::PREC_COMPARISON}
    };

    int tokenIndex = 0;

    bool atEndOfTokens() {
        return (tokenIndex == tokens.size());
    }

    bool tokenClassMatch(std::initializer_list<TokenClass> tokens) {
        for (auto token : tokens) {
            if (fetch().token == token) {
                advance();
                return true;
            }
        }

        return false;
    }

    int advance() { return ++tokenIndex; }

    int getPrecedence(TokenClass token) { 
        return (
            precedences.find(token) != precedences.end() ? precedences[token] : Precedence::PREC_NONE
        );
    }

    TokenInstance fetch() { return (atEndOfTokens() ? TokenInstance { TokenClass::T_EOF, "", 0, 0 } : tokens[tokenIndex]); }
    TokenInstance fetchPrevious() { return (tokens[tokenIndex - 1]); }

    std::shared_ptr<Statement> binary(std::shared_ptr<Statement> lhs, int currentPrecedence) {
        auto oper = fetch();

        while (getPrecedence(oper.token) >= currentPrecedence) {
            advance();
            auto rhs = primary();
            advance();
            auto nextOper = fetch();

            if (getPrecedence(nextOper.token) == Precedence::PREC_NONE) {
                return std::make_shared<Binary>(lhs, rhs, oper);
            }

            while (getPrecedence(nextOper.token) >= getPrecedence(oper.token)) {
                rhs = binary(rhs, getPrecedence(oper.token) + (
                    getPrecedence(nextOper.token) > getPrecedence(oper.token) ? 1 : 0
                ));
                nextOper = fetch();
            }
        }

        return lhs;
    }

    std::optional<std::shared_ptr<Statement>> unary() {
        auto oper = fetch();

        if (tokenClassMatch({ TokenClass::T_MINUS, TokenClass::T_BANG })) {
            auto rhs = expression();
            return std::make_shared<Unary>(rhs, oper);
        }

        return std::nullopt;
    }

    std::shared_ptr<Statement> primary() {
        auto current = fetch();

        switch (current.token) {
            case TokenClass::T_IDENTIFIER:
            case TokenClass::T_NUMBER:
            case TokenClass::T_STRING:
            case TokenClass::T_TRUE:
            case TokenClass::T_FALSE:
            case TokenClass::T_NULL:
                {
                    advance();
                    return std::make_shared<Literal>(fetchPrevious());
                }
            case TokenClass::T_LEFTPAREN:
                {
                    advance();
                    auto expr = expression();
                    advance();

                    if (!tokenClassMatch({ TokenClass::T_RIGHTPAREN })) {
                        throw ParserException("A closing parenthesis ')' was expected but got something else instead...");
                    }

                    return std::make_shared<Grouping>(expr);
                }
                break;
        }

        throw ParserException("An expression was expected but got something else instead...");
    }

    std::shared_ptr<Statement> expression() {
        if (auto isUnary = unary())
            return *isUnary;
    
        auto lhs = primary();

        lhs = binary(lhs, 0);

        return lhs;
    }

public:
    Parser(std::vector<TokenInstance> tokens) : tokens(tokens) {}

    std::vector<std::shared_ptr<Statement>> compile() {
        std::vector<std::shared_ptr<Statement>> statements;

        while (!atEndOfTokens()) {
            std::shared_ptr<Statement> expr;

            expr = expression();

            statements.push_back(expr);
            advance();
        }

        return statements;
    }
};