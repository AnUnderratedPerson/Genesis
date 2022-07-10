#include "Lexer.hpp"

class Expression;

class LiteralValue;
class Binary;
class Unary;
class Grouping;

class Visitpr {
public:
    virtual void visit(Expression&) = 0;
    virtual void visit(LiteralValue&) = 0;
    virtual void visit(Binary&) = 0;
    virtual void visit(Unary&) = 0;
    virtual void visit(Grouping&) = 0;
};