#ifndef CONSTANT_FOLDING_HPP
#define CONSTANT_FOLDING_HPP

#include "ast.hpp"

class ConstantFolder {
private:
    Program newProgram = Program();
    Program program;

    void optimizeStatement(Statement node);
    void optimizeVariableDeclaration(VariableDeclaration node);

public:
    ConstantFolder(Program program);
    ~ConstantFolder();

    Program optimize();
};

#endif // CONSTANT_FOLDING_HPP
