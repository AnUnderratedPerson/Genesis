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

struct ParserException {
    std::string message;

    std::string what() { return message; }
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

    int current = 0;

public:
    Parser(std::vector<TokenInstance> _tokens) : tokens(_tokens) {};

    bool atEnd() {
        return current >= tokens.size();
    }

    bool assert(TokenClass type) {
        if (at().token != type) {
            throw ParserException { format("Expected a different value but got '%s' instead...", { at().value }) };
        }

        return true;
    }

     bool matches(TokenClass current, std::initializer_list<TokenClass> matched) {
        for (auto i : matched) {
            if (i == current)
                return true;
        }

        return false;
     }

    void advanceCurrent() {
        current++;
    }

    TokenInstance at() {
        return tokens[current];
    }

    TokenInstance before() {
        return tokens[current - 1];
    }

    TokenInstance consume() {
        return tokens[current++];
    }

    int getPrecedence(TokenClass token) {
        if (precedences.find(token) == precedences.end())
            return PREC_NONE;

        return (int) precedences.at(token);
    }

    std::shared_ptr<Statement> binary(std::shared_ptr<Statement> lhs, int minPriority) {
        // Gets the precedence for the current operator, if it doesnt exist, the priority is 0
        int currentPriority = getPrecedence(at().token);

        // Compare the current operator precedence with the minimum precedence
        while (currentPriority >= minPriority) {
            // Consume that operator
            auto oper = consume();
            // Get the precedence of the operator
            auto operPriority = getPrecedence(oper.token);
            // Evaluates the rhs of the operator as an expression
            auto rhs = primary();

            // Gets the current precedence of the operator
            currentPriority = getPrecedence(at().token);

            // Checks if the operator exists in the map
            if (currentPriority == PREC_NONE) {
                return std::make_shared<Binary>(lhs, rhs, oper);
            }

            // Checks if the new operator precedence is greater than the previous, if it is then change rhs to a new Binary Expression (so precedence goes to rhs instead)
            while (currentPriority >= getPrecedence(oper.token)) {
                // Change rhs to the new Binary expression to rule precedence
                rhs = binary(rhs, operPriority + (currentPriority > operPriority ? 1 : 0));

                // Gets the current operator after rhs and replaces the old with it
                currentPriority = getPrecedence(at().token);

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
        switch (at().token) {
            case TokenClass::T_NUMBER:
            case TokenClass::T_STRING:
            case TokenClass::T_TRUE:
            case TokenClass::T_FALSE:
            case TokenClass::T_NULL:
            case TokenClass::T_IDENTIFIER:
            {
                advanceCurrent();
                return std::make_shared<LiteralValue>(before());
            }
            break;
        }

        if (at().token == TokenClass::T_LEFTPAREN) {
            advanceCurrent();
            std::shared_ptr<Statement> expr = expression();
            advanceCurrent();

            return std::make_shared<Grouping>(expr);
        }

        throw ParserException { format("Expected an expression but got '%s' instead...", { at().value }) };
    }

    std::shared_ptr<Statement> expression() {
        std::optional<std::shared_ptr<Statement>> lhs = std::nullopt;

        if (matches(at().token, {TokenClass::T_MINUS, TokenClass::T_BANG})) {
            auto oper = consume();
            auto rhs = primary();

            lhs = std::make_shared<Unary>(rhs, oper);
        }

        if (!lhs) {
            lhs = primary();
        }

        lhs = binary(*lhs, 1);

        return *lhs;
    }

    std::vector<std::shared_ptr<Statement>> compile() {
        while (!atEnd()) {
            std::shared_ptr<Statement> expr;

            expr = expression();

            statements.push_back(expr);
            advanceCurrent();
        }

        return statements;
    };
};