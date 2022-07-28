#ifndef UTIL_GENESIS
#define UTIL_GENESIS

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <optional>

class Exception {
public:
    virtual std::string exception() = 0;
};

#endif // UTIL_GENESIS