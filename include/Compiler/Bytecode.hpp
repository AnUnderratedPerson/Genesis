#include "../Util/Util.hpp"

enum class Instruction {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_UNARY,

    OP_EQUALEQUAL,
    OP_NOTEQUAL,
    OP_LESS,
    OP_GREATER,
    OP_LESSEQUAL,
    OP_GREATEREQUAL,

    OP_PUSHVAR,
    OP_FETCHVAR,
    OP_ENTERBLOCK,
    OP_LEAVEBLOCK,

    OP_NONE,
};

enum class ConstantClass {
    CC_BOOLEAN,
    CC_NUMBER,
    CC_NULL,
    CC_STRING,
};

struct Constant {
    ConstantClass type;

    union {
        double number;
        bool boolean;
        int vectorId;
    };
};

struct BytecodeConstructor {
    std::vector<Instruction> instructions;
    std::vector<Constant> constants;
    std::vector<std::string> labels;
};

std::string instructionToString(Instruction instruction) {
    switch (instruction) {
        case Instruction::OP_ADD:
            return "OP_ADD";
        case Instruction::OP_SUB:
            return "OP_SUB";
        case Instruction::OP_MUL:
            return "OP_MUL";
        case Instruction::OP_DIV:
            return "OP_DIV";
        case Instruction::OP_UNARY:
            return "OP_UNARY";
        case Instruction::OP_EQUALEQUAL:
            return "OP_EQUALEQUAL";
        case Instruction::OP_NOTEQUAL:
            return "OP_NOTEQUAL";
        case Instruction::OP_LESS:
            return "OP_LESS";
        case Instruction::OP_GREATER:
            return "OP_GREATER";
        case Instruction::OP_LESSEQUAL:
            return "OP_LESSEQUAL";
        case Instruction::OP_GREATEREQUAL:
            return "OP_GREATEREQUAL";
        case Instruction::OP_PUSHVAR:
            return "OP_PUSHVAR";
        case Instruction::OP_FETCHVAR:
            return "OP_FETCHVAR";
        case Instruction::OP_ENTERBLOCK:
            return "OP_ENTERBLOCK";
        case Instruction::OP_LEAVEBLOCK:
            return "OP_LEAVEBLOCK";
        case Instruction::OP_NONE:
            return "OP_NONE";
        default:
            return "OP_UNKNOWN";
    }
}

std::string constantToString(Constant constant) {
    switch (constant.type) {
        case ConstantClass::CC_BOOLEAN:
            return constant.boolean ? "true" : "false";
        case ConstantClass::CC_NUMBER:
            return std::to_string(constant.number);
        case ConstantClass::CC_NULL:
            return "null";
        case ConstantClass::CC_STRING:
            return "string";
        default:
            return "<unknown constant>";
    }
} 

void debugPrintBytecode(BytecodeConstructor &bytecode) {
    std::stringstream format;

    format << ">> Printing Bytecode Chunk Debug\n";
    format << ">> Instructions:\n";

    for (int i = 0; i < bytecode.instructions.size(); i++) {
        format << ">> \t| " << i << " | Instruction: " << instructionToString(bytecode.instructions[i]) << " || Code: " << (int) bytecode.instructions[i] << "\n";
    }

    format << ">> Constants:\n";
    for (int i = 0; i < bytecode.constants.size(); i++) {
        format << ">> \t| " << i << " | Constant: " << constantToString(bytecode.constants[i]) << "\n";
    }

    format << ">> Locals:\n";
    for (int i = 0; i < bytecode.labels.size(); i++) {
        format << ">> \t| " << i << " | Local: " << bytecode.labels[i] << "\n";
    }

    format << ">> End of Bytecode Chunk Debug\n";
    std::cout << format.str();
}