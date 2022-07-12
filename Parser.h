#pragma once
#include "./state/State.h"

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

class Parser {
private:
     vector<Token> _tokens;
     unordered_map<TokenClass, Precedence> _prototype = {
         {TokenClass::T_PLUS, Precedence::PREC_TERM},
         {TokenClass::T_MINUS, Precedence::PREC_TERM},
         {TokenClass::T_SLASH, Precedence::PREC_FACTOR},
         {TokenClass::T_STAR, Precedence::PREC_FACTOR},
         
         {TokenClass::T_EQUALEQUAL, Precedence::PREC_EQUALITY},
         {TokenClass::T_NOTEQUAL, Precedence::PREC_EQUALITY},
         
         {TokenClass::T_LESS, Precedence::PREC_COMPARISON },
         {TokenClass::T_GREATER, Precedence::PREC_COMPARISON},
         {TokenClass::T_LESSEQUAL, Precedence::PREC_COMPARISON},
         {TokenClass::T_GREATEREQUAL, Precedence::PREC_COMPARISON}
     };
     
     int current = 0;

     void advance() {
         current++;
     }

     void previous() {
         current--;
     }

     void assert(TokenClass expected, string message) {
         if (expected != fetch().type)
             throw ParserException(message);
     }

     bool isEnd() {
         return (current >= _tokens.size());
     }
     
     bool matches(TokenClass current, initializer_list<TokenClass> matched) {
         for (auto i : matched) {
             if (i == current)
                 return true;
         }

         return false;
     }

     int getPrecedence(TokenClass token) {
         if (_prototype.find(token) == _prototype.end())
             return PREC_NONE;
         
         return (int) _prototype.at(token);
     }

     Token fetch() {
         return (isEnd() ? Token(T_NONE, "null") : _tokens.at(current));
     }

     Token consume() {
         Token cache = fetch();
         advance();
         return cache;
     }

     Token before() {
         return _tokens.at(current - 1);
     }
     
     ExpressionPtr primary() {
         switch (fetch().type) {
             case TokenClass::T_NUMBER:
             case TokenClass::T_STRING:
             case TokenClass::T_TRUE:
             case TokenClass::T_FALSE:
             case TokenClass::T_NIL:
             case TokenClass::T_IDENTIFIER:
             {
                 advance();
                 return make_shared<Literal>(before().type, before().value);
             }
         }

         if (fetch().type == TokenClass::T_LPAREN) {
             advance();
             ExpressionPtr expr = expression();
             assert(TokenClass::T_RPAREN, format("ParserException: ')' expected to close group but got '%s' instead?", { fetch().value }));
             advance();
             
             return make_shared<Group>(expr);
         }

         throw ParserException(format("ParserException: Expression expected but got '%s' instead?", { fetch().format() }));
     }

     ExpressionPtr binary(ExpressionPtr lhs, int minPriority) {
         // Gets the precedence for the current operator, if it doesnt exist, the priority is 0
         int currentPriority = getPrecedence(fetch().type);
         
         // Compare the current operator precedence with the minimum precedence
         while (currentPriority >= minPriority) {
             // Consume that operator
             auto oper = consume();
             // Get the precedence of the operator
             auto operPriority = getPrecedence(oper.type);
             // Evaluates the rhs of the operator as an expression
             auto rhs = primary();
             
             // Gets the current precedence of the operator
             currentPriority = getPrecedence(fetch().type);

             // Checks if the operator exists in the map
             if (currentPriority == PREC_NONE) {
                 return make_shared<Binary>(lhs, oper, rhs);
             }

             // Checks if the new operator precedence is greater than the previous, if it is then change rhs to a new Binary Expression (so precedence goes to rhs instead)
             while (currentPriority >= getPrecedence(oper.type)) {
                 // Change rhs to the new Binary expression to rule precedence
                 rhs = binary(rhs, operPriority + (currentPriority > operPriority ? 1 : 0));

                 // Gets the current operator after rhs and replaces the old with it
                 currentPriority = getPrecedence(fetch().type);

                 // Checks if the operator exists in the hashmap
                 if (currentPriority == PREC_NONE) {
                     break;
                 }
             }

             lhs = make_shared<Binary>(lhs, oper, rhs);
         }

         return lhs;
     }

     ExpressionPtr expression() {
         optional<ExpressionPtr> lhs = nullopt;
         
         if (matches(fetch().type, { TokenClass::T_MINUS, TokenClass::T_NOT })) {
             auto oper = consume();
             auto rhs = primary();
             
             lhs = make_shared<Unary>(oper, rhs);
         }
         
         if (!lhs) {
             lhs = primary();
         }

         lhs = binary(*lhs, 1);

         return *lhs;
     }

     StatementPtr variable() {
         advance();
         assert(TokenClass::T_IDENTIFIER, format("ParserException: Expected <identifier> for variable declaration after let but got '%s'?", { fetch().value }));
         
         auto name = consume().value;
         
         assert(TokenClass::T_EQUAL, format("ParserException: Expected '=' after <identifier> but got '%s'?", { fetch().value }));
         advance();
         
         auto value = expression();
         
         return make_shared<Variable>(name, value);
     }

     StatementPtr assignment() {
         auto name = before().value;
         
         advance();
         
         auto value = expression();

         return make_shared<Assignment>(name, value);
     }

     StatementPtr block() {
         vector<StatementPtr> statements;
         
         advance();

         while (fetch().type != TokenClass::T_RBRACE) {
             if (auto expr = run()) {
                 statements.push_back(*expr);
             }
         }

         return make_shared<Block>(statements);
     }
     
     optional<StatementPtr> run() {
         switch (fetch().type) {
             case TokenClass::T_LET:
                 return variable();
             case TokenClass::T_LBRACE:
                 return block();
         }

         StatementPtr expr = expression();
         
         if (matches(fetch().type, { TokenClass::T_SEMICOLON })) {
             consume();

             return nullopt;
         }
         
         if (matches(fetch().type, { TokenClass::T_EQUAL })) {
             if (matches(before().type, { TokenClass::T_IDENTIFIER })) {
                 return assignment();
             }
         }

         if (matches(fetch().type, { TokenClass::T_SEMICOLON })) {
             advance();
         }

         return expr;
     }

public:
    
     Parser(vector<Token> tokens) : _tokens(tokens) {}
     
     vector<StatementPtr> compile() {
         vector<StatementPtr> expressions;

         while (!isEnd()) {
             if (auto expr = run()) {
                 expressions.push_back(*expr);
             }
         }

         return expressions;
     }
};