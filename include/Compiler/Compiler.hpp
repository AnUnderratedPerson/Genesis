#include "../AST/Parser.hpp"
#include "Bytecode.hpp"

class CompilerException : public Exception {
public:
    std::string message;

    CompilerException(std::string message) : message(message) {}

    std::string exception() {
        return message;
    }
};

class Compiler : public Visit {
private:
    BytecodeConstructor bytecode;

    void pushConstant(Constant constant) {
        bytecode.constants.push_back(constant);
    }

    void pushStringConstant(std::string constant) {}
    void pushLabel(std::string name) {
        bytecode.labels.push_back(name);
    }

    void pushInstruction(Instruction instruction) {
        bytecode.instructions.push_back(instruction);
    }

    Instruction assertOperator(TokenInstance token) {
        switch (token.token) {
            case TokenClass::T_PLUS:
                return Instruction::OP_ADD;
            case TokenClass::T_MINUS:
                return Instruction::OP_SUB;
            case TokenClass::T_STAR:
                return Instruction::OP_MUL;
            case TokenClass::T_SLASH:
                return Instruction::OP_DIV;
            case TokenClass::T_EQUALEQUAL:
                return Instruction::OP_EQUALEQUAL;
            case TokenClass::T_NOTEQUAL:
                return Instruction::OP_NOTEQUAL;
            case TokenClass::T_LESS:
                return Instruction::OP_LESS;
            case TokenClass::T_GREATER:
                return Instruction::OP_GREATER;
            case TokenClass::T_LESSEQUAL:
                return Instruction::OP_LESSEQUAL;
            case TokenClass::T_GREATEREQUAL:
                return Instruction::OP_GREATEREQUAL;
            default:
                return Instruction::OP_NONE;
        }
    }

public:
    Compiler() {}

    void visit(Literal &node) {
        auto token = node.token;
        Constant constant;

        switch (token.token) {
            case TokenClass::T_NUMBER:
                {
                    constant.type = ConstantClass::CC_NUMBER;
                    constant.number = std::stod(token.value);
                }
                break;
            case TokenClass::T_STRING:
                break;
            case TokenClass::T_IDENTIFIER:
                pushStringConstant(token.value);
                pushInstruction(Instruction::OP_PUSHVAR);
                return;
            case TokenClass::T_TRUE:
            case TokenClass::T_FALSE:
                {
                    constant.type = ConstantClass::CC_BOOLEAN;
                    constant.boolean = (token.token == TokenClass::T_TRUE) ? true : false;
                }
                break;
            case TokenClass::T_NULL:
                {
                    constant.type = ConstantClass::CC_NULL;
                }
                break;
        }

        pushConstant(constant);
    }

    void visit(Binary &node) {
        node.left->accept(*this);
        node.right->accept(*this);

        pushInstruction(assertOperator(node.op));
    }

    void visit(Unary &node) {
        node.right->accept(*this);
        pushInstruction(Instruction::OP_UNARY);
    }

    void visit(Grouping &node) {
        node.expression->accept(*this);
    }

    void visit(Assignment &node) {
        node.value->accept(*this);

        pushLabel(node.name.value);
        pushInstruction(Instruction::OP_PUSHVAR);
    }

    void visit(Block &node) {
        pushInstruction(Instruction::OP_ENTERBLOCK);

        for (int i = 0; i < node.statements.size(); i++) {
            node.statements[i]->accept(*this);
        }

        pushInstruction(Instruction::OP_LEAVEBLOCK);
    }

    void accept(Statement& node) {
        node.accept(*this);
    }

    BytecodeConstructor getBytecode() { return bytecode; }
};