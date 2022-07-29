#include "Statement.hpp"

// Lower down = Greater precedence
enum Precedence {
    PREC_NONE,        // null
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
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
        {TokenClass::T_GREATEREQUAL, Precedence::PREC_COMPARISON},

        {TokenClass::T_EOF, Precedence::PREC_NONE}
    };

    int tokenIndex = 0;

    bool atEndOfTokens() {
        return (tokenIndex >= tokens.size());
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

    void before() { tokenIndex--; }
    void advance() { tokenIndex++; }

    int getPrecedence(TokenClass token) { 
        return (
            (precedences.find(token) != precedences.end()) ? precedences[token] : Precedence::PREC_NONE
        );
    }

    TokenInstance fetch() { return (atEndOfTokens() ? TokenInstance { TokenClass::T_EOF, "<end of file>", 0, 0 } : tokens[tokenIndex]); }
    TokenInstance fetchPrevious() { return (tokens[tokenIndex - 1]); }

    TokenInstance consume(TokenClass token, std::string message) {
        if (atEndOfTokens())
            throw ParserException(message);

        if (fetch().token != token)
            throw ParserException(message);

        auto cache = fetch();
        advance();
        return cache;
    }

    std::shared_ptr<Expression> binary(std::shared_ptr<Expression> lhs, int currentPrecedence) {
        auto oper = fetch();

        if (getPrecedence(oper.token) == Precedence::PREC_NONE) {
            return lhs;
        }

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

                if (getPrecedence(nextOper.token) == Precedence::PREC_NONE) {
                    return std::make_shared<Binary>(lhs, rhs, oper);
                }
            }

            lhs = std::make_shared<Binary>(lhs, rhs, oper);
            oper = fetch();
        }

        return lhs;
    }

    std::optional<std::shared_ptr<Expression>> unary() {
        auto oper = fetch();

        if (tokenClassMatch({ TokenClass::T_MINUS, TokenClass::T_BANG })) {
            auto rhs = expression();
            advance();
            
            return std::make_shared<Unary>(rhs, oper);
        }

        return std::nullopt;
    }

    std::shared_ptr<Expression> primary() {
        auto current = fetch();

        switch (current.token) {
            case TokenClass::T_IDENTIFIER:
            case TokenClass::T_NUMBER:
            case TokenClass::T_STRING:
            case TokenClass::T_TRUE:
            case TokenClass::T_FALSE:
            case TokenClass::T_NULL:
                {
                    return std::make_shared<Literal>(fetch());
                }
            case TokenClass::T_LEFTPAREN:
                {
                    advance();
                    auto expr = expression();

                    if (!tokenClassMatch({ TokenClass::T_RIGHTPAREN })) {
                        throw ParserException("A closing parenthesis ')' was expected but got something else instead...");
                    }

                    return std::make_shared<Grouping>(expr);
                }
                break;
        }

        throw ParserException("An expression was expected but got something else instead...");
    }

    std::shared_ptr<Expression> expression() {
        if (auto isUnary = unary())
            return *isUnary;
    
        auto lhs = primary();
        advance();

        lhs = binary(lhs, 0);

        return lhs;
    }

    std::shared_ptr<Statement> assignment() {
        auto name = consume(TokenClass::T_IDENTIFIER, "A variable name was expected but got something else instead...");
        consume(TokenClass::T_EQUAL, "An assignment operator '=' was expected but got something else instead...");
        auto value = expression();

        return std::make_shared<Assignment>(name, value);
    }

    std::shared_ptr<Statement> block() {
        std::vector<std::shared_ptr<Statement>> statements;

        while (!atEndOfTokens() && (fetch().token != TokenClass::T_RIGHTBRACE)) {
            statements.push_back(run());
        }

        consume(TokenClass::T_RIGHTBRACE, "A closing brace '}' was expected but got something else instead...");
        return std::make_shared<Block>(statements);
    }

    std::shared_ptr<Statement> run() {
        auto current = fetch();
        std::shared_ptr<Statement> expr;

        switch (current.token) {
            case TokenClass::T_LET:
                advance();
                return assignment();
            case TokenClass::T_LEFTBRACE:
                advance();
                return block();
        }

        expr = expression();
        current = fetch();

        if (current.token == TokenClass::T_EQUAL) {
            before();
            return assignment();
        }

        advance();
        return expr;
    }

public:
    Parser(std::vector<TokenInstance> tokens) : tokens(tokens) {}

    std::vector<std::shared_ptr<Statement>> compile() {
        std::vector<std::shared_ptr<Statement>> statements;

        while (!atEndOfTokens()) {
            statements.push_back(run());
        }

        return statements;
    }
};