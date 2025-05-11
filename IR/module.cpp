#include "IR/module.hpp"

namespace IR{

std::string Module::allocName(std::string base) {
    if(base.empty()) {
        base = std::to_string(names.size());
    }
    std::string new_name = base;
    while(isNameConflict(new_name)) {
        new_name = base + std::to_string(++names.at(base));
    }
    names.insert({base, 0});
}

}