#include <iostream>
#include "constant_folding.hpp"

ConstantFolder::ConstantFolder(Program program) : program(program) {};
ConstantFolder::~ConstantFolder() {};

Expression ConstantFolder::optimizeBinaryOperation(BinaryOperation node) {
    return std::visit([&](auto&& lhs, auto&& rhs) -> Expression {
        using LType = std::decay_t<decltype(lhs)>;
        using RType = std::decay_t<decltype(rhs)>;

        if constexpr (
            (std::is_same_v<LType, IntLiteral> || std::is_same_v<LType, FloatLiteral>) &&
            (std::is_same_v<RType, IntLiteral> || std::is_same_v<RType, FloatLiteral>)
        ) {
            std::optional<Expression> expr;
            if (
                (node.op == "*" && lhs.value != 0 && rhs.value != 0) &&
                (node.op == "/" || std::is_same_v<LType, FloatLiteral> || std::is_same_v<RType, FloatLiteral>)
            ) {
                float value;
                if (node.op == "+") {
                    value = lhs.value + rhs.value;
                } else if (node.op == "-") {
                    value = lhs.value - rhs.value;
                } else if (node.op == "*") {
                    value = lhs.value * rhs.value;
                } else if (node.op == "/") {
                    value = lhs.value / rhs.value;
                } /* else if (node.op == "%") {
                    value = lhs.value % rhs.value;
                } */ else {
                    std::cerr << "Unknown op in constant folding" << std::endl;
                }

                expr = FloatLiteral(node.metadata, value);
            } else {
                int value;
                if (node.op == "+") {
                    value = lhs.value + rhs.value;
                } else if (node.op == "-") {
                    value = lhs.value - rhs.value;
                } else if (node.op == "*") {
                    value = lhs.value * rhs.value;
                } else if (node.op == "/") {
                    value = lhs.value / rhs.value;
                } /* else if (node.op == "%") {
                    value = lhs.value % rhs.value;
                } */ else {
                    std::cerr << "Unknown op in constant folding" << std::endl;
                }

                expr = IntLiteral(node.metadata, value);
            }

            return expr.value();
        } else if (
        (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, StringLiteral> && node.op == "+") ||
            (((std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, IntLiteral>) ||
        (std::is_same_v<LType, IntLiteral> && std::is_same_v<RType, StringLiteral>)) && node.op == "*")
        ) {
            // * The conditions are constexpr so that .value if inferred correctly
            // * Otherwise, I get C2039 errors

            std::optional<Expression> expr;
            if constexpr (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, StringLiteral>) {
                expr = StringLiteral(node.metadata, lhs.value + rhs.value);
            } else {
                std::string str;
                int max;
                if constexpr (std::is_same_v<LType, IntLiteral> && std::is_same_v<RType, StringLiteral>) {
                    max = lhs.value;
                    str = rhs.value;
                } else if constexpr (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, IntLiteral>) {
                    max = rhs.value;
                    str = lhs.value;
                }

                std::string buffer = "";
                for (size_t i = 0; i < max; i++)
                {
                    buffer += str;
                }

                expr = StringLiteral(node.metadata, buffer);
            }

            return expr.value();
        }
    }, *node.lhs, *node.rhs);
};

Expression ConstantFolder::optimizeExpression(Expression node) {
    return std::visit([&](const auto& expression) -> Expression {
        using ExpressionType = std::decay_t<decltype(expression)>;

        if constexpr (std::is_same_v<ExpressionType, BinaryOperation>) {
            return this->optimizeBinaryOperation(expression);
        } else {
            return node;
        }
    }, node);
};

Statement ConstantFolder::optimizeVariableDeclaration(VariableDeclaration node) {
    if (!node.value.has_value()) {
        return node;
    }

    return std::visit([&](const auto& expression) -> Statement {
        using ExpressionType = std::decay_t<decltype(expression)>;

        if constexpr (std::is_same_v<ExpressionType, BinaryOperation>) {
            return VariableDeclaration(
                node.metadata,
                node.isMutable,
                node.type,
                node.identifier,
                std::make_shared<Expression>(this->optimizeBinaryOperation(expression))
            );
        } else {
            return node;
        }
    }, *(node.value.value()));
};

Statement ConstantFolder::optimizeStatement(Statement node) {
    return std::visit([&](const auto& statement) -> Statement {
        using NodeType = std::decay_t<decltype(statement)>;

        if constexpr (std::is_same_v<NodeType, VariableDeclaration>) {
            return this->optimizeVariableDeclaration(statement);
        } else {
            return statement;
        }
    }, node);
}

Program ConstantFolder::optimize() {
    for (size_t i = 0; i < this->program.body.size(); i++)
    {
        auto statementOrExpression = std::visit([&](const auto& node) -> std::variant<Expression, Statement> {
            using NodeType = std::decay_t<decltype(*node)>;

            if constexpr (std::is_same_v<NodeType, Statement>) {
                return this->optimizeStatement(*node);
            } else {
                return this->optimizeExpression(*node);
            }
        }, this->program.body.at(i));

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            this->newProgram.body.push_back(
                std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            this->newProgram.body.push_back(
                std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    return this->newProgram;
};
