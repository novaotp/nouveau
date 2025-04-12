#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <string>
#include "ast.hpp"

class Codegen {
private:
    Program program;

    std::string writeAssembly();

public:
    Codegen(Program program);
    ~Codegen();

    // Generates assembly code.
    void generate();
};

#endif // CODEGEN_HPP
