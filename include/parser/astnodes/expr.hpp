#ifndef __EXPR_H
#define __EXPR_H

#include "parser/astnodes/node.hpp"

struct expr : public node {
    public:
        expr(size_t row, size_t col) 
        : node(row, col) {}
    public:
        // type type;
};

#endif