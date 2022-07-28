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
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
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
        {TokenClass::T_GREATEREQUAL, Precedence::PREC_COMPARISON}};

    int tokenIndex = 0;

    void advance() {
        tokenIndex++;
    }

    bool atEnd() {
        return tokenIndex >= tokens.size();
    }

    bool assert(TokenClass type) {
        if (atEnd()) {
            return false;
        }

        if (current().token == type) {
            return true;
        }

        return false;
    }

    bool matches(std::initializer_list<TokenClass> types, TokenInstance token) {
        if (atEnd()) {
            return false;
        }

        for (auto type : types) {
            if (token.token == type) {
                return true;
            }
        }

        return false;
    }

    TokenInstance consume(TokenClass type, std::string message) {
        if (atEnd())
            throw ParserException(message);

        if (current().token == type) {
            return tokens[tokenIndex++];
        }

        throw ParserException(message);
    }

    TokenInstance consume(std::initializer_list<TokenClass> types, std::string message) {
        if (atEnd())
            throw ParserException(message);

        for (auto type : types) {
            if (current().token == type) {
                return tokens[tokenIndex++];
            }
        }

        throw ParserException(message);
    }

    TokenInstance current() {
        return tokens[tokenIndex];
    }

    TokenInstance before() {
        return tokens[tokenIndex - 1];
    }

    int getPrecedence(TokenClass token) {
        if (precedences.find(token) == precedences.end())
            return PREC_NONE;

        return (int) precedences.at(token);
    }

    std::shared_ptr<Statement> binary(std::shared_ptr<Statement> lhs, int minPriority) {
        // Gets the precedence for the current operator, if it doesnt exist, the priority is 0
        int currentPriority = getPrecedence(current().token);

        // Compare the current operator precedence with the minimum precedence
        while (currentPriority >= minPriority) {
            // Consume that operator
            auto oper = current();
            // Get the precedence of the operator
            auto operPriority = getPrecedence(oper.token);
            // Evaluates the rhs of the operator as an expression
            auto rhs = primary();

            // Gets the current precedence of the operator
            currentPriority = getPrecedence(current().token);

            // Checks if the operator exists in the map
            if (currentPriority == PREC_NONE) {
                return std::make_shared<Binary>(lhs, rhs, oper);
            }

            // Checks if the new operator precedence is greater than the previous, if it is then change rhs to a new Binary Expression (so precedence goes to rhs instead)
            while (currentPriority >= getPrecedence(oper.token)) {
                // Change rhs to the new Binary expression to rule precedence
                rhs = binary(rhs, operPriority + (currentPriority > operPriority ? 1 : 0));

                // Gets the current operator after rhs and replaces the old with it
                currentPriority = getPrecedence(current().token);

                // Checks if the operator exists in the hashmap
                if (currentPriority == PREC_NONE) {
                    break;
                }
            }

            lhs = std::make_shared<Binary>(lhs, rhs, oper);
        }

        return lhs;
    }

    std::shared_ptr<Statement> primary() {
        switch (current().token) {
            case TokenClass::T_NUMBER:
            case TokenClass::T_STRING:
            case TokenClass::T_TRUE:
            case TokenClass::T_FALSE:
            case TokenClass::T_NULL:
            case TokenClass::T_IDENTIFIER:
                {
                    advance();
                    return std::make_shared<LiteralValue>(before());
                }
                break;
        }

        if (current().token == TokenClass::T_LEFTPAREN) {
            advance();
            auto expr = expression();
            advance();

            return std::make_shared<Grouping>(expr);
        }

        std::stringstream str;

        str << "Genesis | ParserException:\n"
            << "  Line: " << current().line << "\n"
            << "  Expected an expression but got '" << current().value << "' instead?\n";

        throw LexerException(str.str());
    }

    std::shared_ptr<Statement> expression() {
        std::optional<std::shared_ptr<Statement>> lhs = std::nullopt;

        if (matches({TokenClass::T_MINUS, TokenClass::T_BANG}, current())) {
            auto oper = current();
            auto rhs = primary();

            lhs = std::make_shared<Unary>(rhs, oper);
        }

        if (!lhs) {
            lhs = primary();
        }

        lhs = binary(*lhs, 1);

        return *lhs;
    }

    std::shared_ptr<Statement> declaration() {
        advance();

        auto name = consume(TokenClass::T_IDENTIFIER, "Genesis | ParserException:\n  Expected an identifier after 'let'");

        assert(TokenClass::T_EQUAL);
        advance();

        auto value = expression();

        return std::make_shared<LocalAssignment>(name, value);
    }

public:
    Parser(std::vector<TokenInstance> _tokens) : tokens(_tokens){};

    std::vector<std::shared_ptr<Statement>> compile() {
        while (!atEnd()) {
            std::shared_ptr<Statement> expr;

            switch (current().token) {
                case TokenClass::T_LET:
                    {
                        statements.push_back(declaration());
                        continue;
                    }
                    break; 
            }

            expr = expression();

            statements.push_back(expr);
            advance();
        }

        return statements;
    };
};