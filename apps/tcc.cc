#include <iostream>

#include "tcc/parser/parser.h"

int main() {
    std::cout << "hello world" << std::endl;
    tcc::parser::ParseFrozenGraph("TEST-PATH");
}
