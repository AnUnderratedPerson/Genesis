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

#define debug(...) std::cout << __VA_ARGS__ << std::endl;

using formatValues = std::variant<int, double, char, const char*, std::string, bool>;

std::string format(std::string input, std::vector<formatValues> args) {
    std::string output = input;
    int current = 0, argc = 0;

    while (current < input.size()) {
        if (input[current] == '%') {
            // Check if the next character is a valid format specifier
            if (current + 1 >= input.size())
                break;

            if (argc >= args.size())
                break;

            switch (input[current + 1]) {
                case 'd':
                    {
                        double _in = std::get<double>(args.at(argc));
                        output.replace(current - 1, 2, std::to_string(_in));
                        argc++;
                    }
                    break;
                case 'i':
                    {
                        int _in = std::get<int>(args.at(argc));
                        output.replace(current - 1, 2, std::to_string(_in));
                        argc++;
                    }
                    break;
                case 'c':
                    {
                        char _in = std::get<char>(args.at(argc));
                        output.replace(current - 1, 2, std::string(1, _in));
                        argc++;
                    }
                    break;
                case 's':
                    {
                        try {
                            const char* _in = std::get<const char*>(args.at(argc));
                            output.replace(current - 1, 2, _in);
                            argc++;
                        }
                        catch(...) {
                            try {
                                std::string _in = std::get<std::string>(args.at(argc));
                                output.replace(current - 1, 2, _in);
                                argc++;
                            }
                            catch(...) {}
                        }
                    }
                    break;
                case 'b':
                    {
                        bool _in = std::get<bool>(args.at(argc));
                        output.replace(current - 1, 2, (_in ? "true" : "false"));
                        argc++;
                    }
                    break;
                default:
                    break;
            }
        }

        current++;
    }

    return output;
}

#endif // UTIL_GENESIS