#include "../include/AST/Parser.hpp"

class CoreException : public Exception {
public:
    std::string message;

    CoreException(std::string message) : message(message) {}

    std::string exception() {
        return message;
    }
};

class CoreInstance {
public:
    CoreInstance() {}

    std::string readFile(std::string path) {
        std::fstream inputFile;
        std::stringstream buffer;

        inputFile.open(path, std::ios::in);

        if (!inputFile.is_open()) {
            std::stringstream str;

            str << "Could not open file at path:\n  " 
                << path;
            
            throw CoreException(str.str());
        }

        buffer << inputFile.rdbuf();
        inputFile.close();

        return buffer.str();
    }

    void run(std::string path) {
        std::string source = readFile(path);

        Lexer lexer(source);
        Parser parser(lexer.compile());
        auto AST = parser.compile();

        for (auto &node : AST) {
            std::cout << node->toString() << std::endl;
        }
    }
};

int main(int charc, char** argv) {
    if (charc < 2) {
        std::cout << "Usage: " << argv[0] << " <path>" << std::endl;
        return 1;
    }

    CoreInstance core;
    
    try {
        core.run(argv[1]);
        return 0;
    } catch (Exception& e) {
        std::cerr << e.exception() << std::endl;
        return 1;
    }
}