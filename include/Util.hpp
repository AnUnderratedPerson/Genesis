#ifndef GENESIS_IMPORT_NAMESPACE
#define GENESIS_IMPORT_NAMESPACE

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <variant>

struct CoreException {
    std::string message;

    void what() {
        std::cerr << "[Core Exception]: >> " << message << std::endl;
    }
};

#endif

// --> TODO: formatting for internal use

