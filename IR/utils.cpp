#include "IR/utils.hpp"

void __assert__(bool cond, const std::string &str) {
    if(!cond) {
        std::cout << str << "\n";
        exit(1);
    }
}