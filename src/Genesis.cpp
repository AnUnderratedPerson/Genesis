#include "../include/AST/Lexer.hpp"

struct GPair {
    bool success;
    std::string response;
};

GPair fetchContent(int count, char** argv) {
    if (count <= 1) {
        std::cerr
            << ">> Genesis:\n"
            << "No file path found or supplied to compiler...\n";

        return {false, ""};
    }

    const char *filePath = argv[1];
    std::fstream inputFile;

    inputFile.open(filePath, std::ios::in);

    if (!inputFile.is_open()) {
        std::cerr
            << ">> Genesis:\n"
            << "Could not open file at path '" << filePath << "'...\n";

        return {false, ""};
    }

    std::stringstream buffer;

    buffer << inputFile.rdbuf();
    inputFile.close();

    return {true, buffer.str()};
}

int main(int charc, char** argv) {
    GPair sourceCode = fetchContent(charc, argv);

    if (!sourceCode.success)
        return 1;

    Lexer lexer(sourceCode.response);
    std::vector<TokenInstance> tokens;

    try {
        tokens = lexer.compile();
    }
    catch(LexerException& e) {
        std::cerr
            << ">> GenesisException:\n"
            << ">> Message: " << e.message << "\n"
            << ">> Line: " << e.line << "\n";

        return 1;
    }

    for (auto i : tokens) {
        std::cout
            << ">> Value: " << i.value << "\n";
    }
}