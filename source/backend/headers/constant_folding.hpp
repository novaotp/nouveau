#ifndef CONSTANT_FOLDING_HPP
#define CONSTANT_FOLDING_HPP

#include "ast.hpp"
#include "semer.hpp"

class ConstantFolder {
private:
    /// @brief If it is `false` after looping over the whole program, optimizations have been made in the last iteration.
    ///
    /// We want to keep looping until no more optimizations can be made.
    bool isOptimized = false;

    Program program;
    std::shared_ptr<Scope> scope;

    Expression optimizeExpression(Expression node);
    Expression optimizeBinaryOperation(BinaryOperation node);
    Expression optimizeLogicalNotOperation(LogicalNotOperation node);
    Expression optimizeIdentifier(Identifier node);

    Statement optimizeStatement(Statement node);
    Statement optimizeVariableDeclaration(VariableDeclaration node);
    Statement optimizeVariableAssignment(VariableAssignment node);

public:
    ConstantFolder(Program program, std::shared_ptr<Scope> scope);
    ~ConstantFolder();

    Program optimize();
};

#endif // CONSTANT_FOLDING_HPP
