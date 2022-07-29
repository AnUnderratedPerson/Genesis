#include "Lexer.hpp"

class Expression;
class Statement;

class Literal;
class Binary;
class Unary;
class Grouping;
class Call;

class Assignment;
class Block;

class Visit {
public:
    virtual void visit(Literal&) = 0;
    virtual void visit(Binary&) = 0;
    virtual void visit(Unary&) = 0;
    virtual void visit(Grouping&) = 0;

    virtual void visit(Assignment &) = 0;
    virtual void visit(Block &) = 0;
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

class Literal : public Expression {
public:
    TokenInstance token;

    Literal(TokenInstance _token) : token(_token) {};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return token.value;
    }
};

class Binary : public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
    TokenInstance op;

    Binary(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right, TokenInstance _op) : left(_left), right(_right), op(_op){};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "Binary: (" + left->toString() + " " + op.value + " " + right->toString() + ")";
    }
};

class Unary : public Expression {
public:
    std::shared_ptr<Expression> right;
    TokenInstance op;

    Unary(std::shared_ptr<Expression> _right, TokenInstance _op) : right(_right), op(_op){};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "Unary: (" + op.value + right->toString() + ")";
    }
};

class Grouping : public Expression {
public:
    std::shared_ptr<Expression> expression;

    Grouping(std::shared_ptr<Expression> _expression) : expression(_expression){};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "Group: (" + expression->toString() + ")";
    }
};

class Assignment : public Statement {
public:
    TokenInstance name;
    std::shared_ptr<Expression> value;

    Assignment(TokenInstance _name, std::shared_ptr<Expression> _value) : name(_name), value(_value){};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        return "(Assignment) " + name.value + " = " + value->toString();
    }
};

class Block : public Statement {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    Block(std::vector<std::shared_ptr<Statement>> _statements) : statements(_statements) {};

    void accept(Visit &visitor) {
        visitor.visit(*this);
    };

    std::string toString() {
        std::string result = "Block: {\n";

        for (auto statement : statements) {
            result += "\t" + statement->toString() + "\n";
        }

        result += "}";
        return result;
    }
};