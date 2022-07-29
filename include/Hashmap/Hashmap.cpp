#include "../Util/Util.hpp"

template <typename T>
class Hashmap {
private:
    struct HashmapNode {
        std::string key;
        T value;
    };

    int hash(std::string str) {
        int hash = 0;

        for (int i = 0; i < str.length(); i++) {
            hash += (int) str[i];
            hash = hash ^ 2;
        }

        return hash;
    }

public:
    void insert(std::stringstring str, T value) {
        int hash = this->hash(str);
        
        if ()
    }

    std::optional<T> request(std::String) {
        return std::nullopt;
    }
};