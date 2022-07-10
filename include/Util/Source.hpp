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
#include <variant>

using formatValues = std::variant<int, double, char, const char*, std::string, bool>;

std::string format(std::string input, std::vector<formatValues> args) {
    std::string output = input;
    int current = 0, argc = 0;

    while (current < input.size()) {
        if (input[current] == '%') {
            // Check if the next character is a valid format specifier
            current++;

            if (current >= input.size())
                break;

            if (argc >= args.size())
                break;

            switch (input[current]) {
                case 'd':
                    {
                        double _in = std::get<double>(args.at(argc));
                        output.replace(current - 1, 2, std::to_string(_in));
                    }
                    break;
                case 'i':
                    {
                        int _in = std::get<int>(args.at(argc));
                        output.replace(current - 1, 2, std::to_string(_in));
                    }
                    break;
                case 'c':
                    {
                        char _in = std::get<char>(args.at(argc));
                        output.replace(current - 1, 2, std::string(1, _in));
                    }
                    break;
                case 's':
                    {
                        try {
                            const char* _in = std::get<const char*>(args.at(argc));
                            output.replace(current - 1, 2, _in);
                        }
                        catch(...) {
                            try {
                                std::string _in = std::get<std::string>(args.at(argc));
                                output.replace(current - 1, 2, _in);
                            }
                            catch(...) {}
                        }
                    }
                    break;
                case 'b':
                    {
                        bool _in = std::get<bool>(args.at(argc));
                        output.replace(current - 1, 2, (_in ? "true" : "false"));
                    }
                    break;
                default:
                    break;
            }

            current++;
            argc++;
        }
    }

    return output;
}

#endif // UTIL_GENESIS