#include "parser/astnodes/node.hpp"

std::string locToString(std::pair<size_t, size_t> location) {
    return " (line " + std::to_string(location.first) + ", col " + std::to_string(location.second) + ")\n";
}

