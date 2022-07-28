#include "Lexer.hpp"

class Expression;
class Statement;

class LiteralValue;
class Binary;
class Unary;
class Grouping;

class LocalAssignment;

class Visit {
public:
    virtual void visit(Expression&) = 0;
    virtual void visit(LiteralValue&) = 0;
    virtual void visit(Binary&) = 0;
    virtual void visit(Unary&) = 0;
    virtual void visit(Grouping&) = 0;

    virtual void visit(LocalAssignment &) = 0;
};

class Statement {
public:
    virtual void accept(Visit &) = 0;
    virtual std::string toString() = 0;
};

class Expression : public Statement {
public:
    virtual void accept(Visit &) = 0;
    virtual std::string toString() = 0;
};

class LiteralValue : public Expression {
public:
    TokenInstance token;

    LiteralValue(TokenInstance _token) : token(_token) {};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return token.value;
    }
};

class Binary : public Expression {
public:
    std::shared_ptr<Statement> left;
    std::shared_ptr<Statement> right;
    TokenInstance op;

    Binary(std::shared_ptr<Statement> _left, std::shared_ptr<Statement> _right, TokenInstance _op) : left(_left), right(_right), op(_op) {};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "(" + left->toString() + " " + op.value + " " + right->toString() + ")";
    }
};

class Unary : public Expression {
public:
    std::shared_ptr<Statement> right;
    TokenInstance op;

    Unary(std::shared_ptr<Statement> _right, TokenInstance _op) : right(_right), op(_op) {};
    
    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "(" + op.value + right->toString() + ")";
    }
};

class Grouping : public Expression {
public:
    std::shared_ptr<Statement> expression;

    Grouping(std::shared_ptr<Statement> _expression) : expression(_expression) {};
    
    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "(" + expression->toString() + ")";
    }
};

class LocalAssignment : public Statement {
public:
    TokenInstance name;
    std::shared_ptr<Statement> value;

    LocalAssignment(TokenInstance _name, std::shared_ptr<Statement> _value) : name(_name), value(_value) {};
    
    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "localAssignment " + name.value + " = " + value->toString();
    }
};