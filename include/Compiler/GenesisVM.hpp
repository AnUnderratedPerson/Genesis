#include "Compiler.hpp"

class GenesisRuntimeException : public Exception {
public:
    std::string message;

    GenesisRuntimeException(std::string message) : message(message) {}

    std::string exception() {
        return message;
    }
};

class Genesis {
private:
    BytecodeConstructor& bytecode;
    size_t ip = 0;

public:
    Genesis(BytecodeConstructor& constructor) : bytecode(constructor) {}

    bool atEnd() { return (ip >= bytecode.instructions.size()); }
    Instruction fetch() { return bytecode.instructions[ip]; }
    void advance() { ip++; }

    int pushConstant(Constant constant) {
        bytecode.constants.push_back(constant);
        return bytecode.constants.size() - 1;
    }

    Constant popConstant() {
        auto constant = bytecode.constants.front();
        bytecode.constants.erase(bytecode.constants.begin());
        return constant;
    }

    std::string popLabel() {
        auto label = bytecode.labels.front();
        bytecode.labels.erase(bytecode.labels.begin());
        return label;
    } 

    void compile() {
        while (!atEnd()) {
            switch (fetch()) {
                case Instruction::OP_ADD:
                    {
                        auto lhs = popConstant(), rhs = popConstant();
                        Constant resultant;

                        if (lhs.type != rhs.type) {
                            throw GenesisRuntimeException("Attempted to perform addition (+) on constants that are not the same type");
                        }

                        switch (lhs.type) {
                            case ConstantClass::CC_NUMBER:
                                {
                                    resultant.type = ConstantClass::CC_NUMBER;
                                    resultant.number = lhs.number + rhs.number;
                                }
                                break;
                            case ConstantClass::CC_STRING:
                                break;
                            default:
                                throw GenesisRuntimeException("Attempted to perform addition (+) on non-numeric constants");
                        }

                        pushConstant(resultant);
                    }
                    break;
                case Instruction::OP_SUB:
                    {
                        auto lhs = popConstant(), rhs = popConstant();
                        Constant resultant;

                        if (lhs.type != rhs.type) {
                            throw GenesisRuntimeException("Attempted to perform subtraction (-) on constants that are not the same type");
                        }

                        switch (lhs.type) {
                            case ConstantClass::CC_NUMBER:
                                {
                                    resultant.type = ConstantClass::CC_NUMBER;
                                    resultant.number = lhs.number - rhs.number;
                                }
                                break;
                            default:
                                throw GenesisRuntimeException("Attempted to perform subtraction (-) on non-numeric constants");
                        }

                        pushConstant(resultant);
                    }
                    break;
                case Instruction::OP_MUL:
                    {
                        auto lhs = popConstant(), rhs = popConstant();
                        Constant resultant;

                        if (lhs.type != rhs.type) {
                            throw GenesisRuntimeException("Attempted to perform multiplication (*) on constants that are not the same type");
                        }

                        switch (lhs.type) {
                            case ConstantClass::CC_NUMBER:
                                {
                                    resultant.type = ConstantClass::CC_NUMBER;
                                    resultant.number = lhs.number * rhs.number;
                                }
                                break;
                            default:
                                throw GenesisRuntimeException("Attempted to perform multiplication (*) on non-numeric constants");
                        }

                        pushConstant(resultant);
                    }
                    break;
                case Instruction::OP_DIV:
                    {
                        auto lhs = popConstant(), rhs = popConstant();
                        Constant resultant;

                        if (lhs.type != rhs.type) {
                            throw GenesisRuntimeException("Attempted to perform division (/) on constants that are not the same type");
                        }

                        switch (lhs.type) {
                            case ConstantClass::CC_NUMBER:
                                {
                                    resultant.type = ConstantClass::CC_NUMBER;
                                    resultant.number = lhs.number / rhs.number;
                                }
                                break;
                            default:
                                throw GenesisRuntimeException("Attempted to perform division (/) on non-numeric constants");
                        }

                        pushConstant(resultant);
                    }
                    break;
                case Instruction::OP_PUSHVAR:
                    {
                        auto value = popConstant();
                        auto name = popLabel();
                    }
                    break;
            }

            ip++;
        }
    }
};