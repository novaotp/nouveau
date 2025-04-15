#ifndef CONSTANT_FOLDING_HPP
#define CONSTANT_FOLDING_HPP

#include "ast.hpp"

class ConstantFolder {
private:
    bool isOptimized = false;
    Program program;

    Expression optimizeExpression(Expression node);
    Expression optimizeBinaryOperation(BinaryOperation node);
    Expression optimizeLogicalNotOperation(LogicalNotOperation ode);

    Statement optimizeStatement(Statement node);
    Statement optimizeVariableDeclaration(VariableDeclaration node);

public:
    ConstantFolder(Program program);
    ~ConstantFolder();

    Program optimize();
};

#endif // CONSTANT_FOLDING_HPP
