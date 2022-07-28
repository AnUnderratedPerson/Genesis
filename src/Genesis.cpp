#include <chrono>

#include "../include/Util/Util.hpp"

/*
std::string readSource(std::string path) {
    std::fstream file;
    std::stringstream ss;

    file.open(path, std::ios_base::in);

    if (!file.is_open()) {
        throw CoreException { "The file supplied for reading is not found at that path..." };
    }

    ss << file.rdbuf();
    file.close();
    
    return ss.str();
}

int codeProcessing() {
    std::string source;

    try {
        source = readSource("internal/ProcessingTest.gen");
    }
    catch (CoreException &e) {
        e.what();
        return 1;
    }

    Lexer lexer(source);
    std::vector<TokenInstance> tokens;

    try {
        tokens = lexer.compile();
    }
    catch (LexerException &e) {
        e.what();
        return 1;
    }

    std::cout << ">> BEFORE PRE CODE PROCESSING <<" << std::endl;

    Processor processor(tokens, lexer.processorItems);
    std::chrono::duration<double, std::milli> duration;

    try {
        auto now = std::chrono::system_clock::now();
        tokens = processor.process();
        auto later = std::chrono::system_clock::now();
        duration = std::chrono::duration<double, std::milli>(later - now);
    }
    catch (ProcessException &e) {
        e.what();
        return 1;
    }

    std::cout << ">> AFTER PRE CODE PROCESSING <<" << std::endl;
    std::cout << ">> PROCESSING TIME: " << duration.count() << "ms <<" << std::endl;

    for (auto &token : tokens) {
        std::cout << "TokenValue: " << token.tokenValue << std::endl;
    }

    return 0;
}
*/

int main() {}