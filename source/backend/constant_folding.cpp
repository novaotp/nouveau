#include <iostream>
#include "constant_folding.hpp"

ConstantFolder::ConstantFolder(Program program, std::shared_ptr<Scope> scope) : program(program), scope(scope) {};
ConstantFolder::~ConstantFolder() {};

Expression ConstantFolder::optimizeIdentifier(Identifier node) {
    auto symbol = this->scope->find(node.name);

    if (symbol == nullptr || symbol->value->isMutable || !symbol->value->value.has_value()) return node;

    return std::visit([&](auto&& expression) -> Expression {
        using ExpressionType = std::decay_t<decltype(expression)>;

        if constexpr (std::is_same_v<ExpressionType, StringLiteral>) {
            this->isOptimized = false;
            return StringLiteral(node.metadata, expression.value);
        } else if constexpr (std::is_same_v<ExpressionType, IntLiteral>) {
            this->isOptimized = false;
            return IntLiteral(node.metadata, expression.value);
        } else if constexpr (std::is_same_v<ExpressionType, FloatLiteral>) {
            this->isOptimized = false;
            return FloatLiteral(node.metadata, expression.value);
        } else if constexpr (std::is_same_v<ExpressionType, BooleanLiteral>) {
            this->isOptimized = false;
            return BooleanLiteral(node.metadata, expression.value);
        } else {
            return node;
        }
    }, *symbol->value->value.value());
}

Expression ConstantFolder::optimizeLogicalNotOperation(LogicalNotOperation node) {
    return std::visit([&](auto&& expression) -> Expression {
        using ExpressionType = std::decay_t<decltype(expression)>;

        if constexpr (std::is_same_v<ExpressionType, BooleanLiteral>) {
            this->isOptimized = false;
            return BooleanLiteral(node.metadata, !expression.value);
        } else if constexpr (
            std::is_same_v<ExpressionType, StringLiteral> ||
            std::is_same_v<ExpressionType, IntLiteral> ||
            std::is_same_v<ExpressionType, FloatLiteral>
        ) {
            // * The only falsy values are false and null

            this->isOptimized = false;
            return BooleanLiteral(node.metadata, false);
        } else if constexpr (std::is_same_v<ExpressionType, LogicalNotOperation>) {
            return LogicalNotOperation(
                node.metadata,
                std::make_shared<Expression>(this->optimizeLogicalNotOperation(expression))
            );
        } else if constexpr (std::is_same_v<ExpressionType, BinaryOperation>) {
            return LogicalNotOperation(
                node.metadata,
                std::make_shared<Expression>(this->optimizeBinaryOperation(expression))
            );
        } else if constexpr (std::is_same_v<ExpressionType, Identifier>) {
            return LogicalNotOperation(
                node.metadata,
                std::make_shared<Expression>(this->optimizeIdentifier(expression))
            );
        } else {
            return expression;
        }
    }, *node.expression);
};

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
                (!(node.op == "*" && (lhs.value == 0 || rhs.value == 0))) &&
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
                    value = float(lhs.value) / rhs.value;
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
                } /* else if (node.op == "%") {
                    value = lhs.value % rhs.value;
                } */ else {
                    std::cerr << "Unknown op in constant folding" << std::endl;
                }

                expr = IntLiteral(node.metadata, value);
            }

            this->isOptimized = false;
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

            this->isOptimized = false;
            return expr.value();
        } else if constexpr (std::is_same_v<LType, BinaryOperation>) {
            return BinaryOperation(
                       node.metadata,
                       std::make_shared<Expression>(this->optimizeBinaryOperation(lhs)),
                       node.op,
                       std::make_shared<Expression>(rhs)
                   );
        } else if constexpr (std::is_same_v<RType, BinaryOperation>) {
            return BinaryOperation(
                       node.metadata,
                       std::make_shared<Expression>(lhs),
                       node.op,
                       std::make_shared<Expression>(this->optimizeBinaryOperation(rhs))
                   );
        } else if constexpr (std::is_same_v<LType, Identifier>) {
            return BinaryOperation(
                       node.metadata,
                       std::make_shared<Expression>(this->optimizeIdentifier(lhs)),
                       node.op,
                       std::make_shared<Expression>(rhs)
                   );
        } else if constexpr (std::is_same_v<RType, Identifier>) {
            return BinaryOperation(
                       node.metadata,
                       std::make_shared<Expression>(lhs),
                       node.op,
                       std::make_shared<Expression>(this->optimizeIdentifier(rhs))
                   );
        } else {
            return node;
        }
    }, *node.lhs, *node.rhs);
};

Expression ConstantFolder::optimizeExpression(Expression node) {
    return std::visit([&](const auto& expression) -> Expression {
        using ExpressionType = std::decay_t<decltype(expression)>;

        if constexpr (std::is_same_v<ExpressionType, BinaryOperation>) {
            return this->optimizeBinaryOperation(expression);
        } else if constexpr (std::is_same_v<ExpressionType, LogicalNotOperation>) {
            return this->optimizeLogicalNotOperation(expression);
        } else if constexpr (std::is_same_v<ExpressionType, Identifier>) {
            return this->optimizeIdentifier(expression);
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
        return VariableDeclaration(
            node.metadata,
            node.isMutable,
            node.type,
            node.identifier,
            std::make_shared<Expression>(this->optimizeExpression(expression))
        );
    }, *(node.value.value()));
};

Statement ConstantFolder::optimizeVariableAssignment(VariableAssignment node) {
    return std::visit([&](const auto& expression) -> Statement {
        return VariableAssignment(
            node.metadata,
            node.identifier,
            node.op,
            std::make_shared<Expression>(this->optimizeExpression(expression))
        );
    }, *node.value);
};

Statement ConstantFolder::optimizeStatement(Statement node) {
    return std::visit([&](const auto& statement) -> Statement {
        using NodeType = std::decay_t<decltype(statement)>;

        if constexpr (std::is_same_v<NodeType, VariableDeclaration>) {
            return this->optimizeVariableDeclaration(statement);
        } else if constexpr (std::is_same_v<NodeType, VariableAssignment>) {
            return this->optimizeVariableAssignment(statement);
        } else {
            return statement;
        }
    }, node);
}

Program ConstantFolder::optimize() {
    Program oldProgram = this->program;
    Program newProgram = Program();
    size_t count = 1;

    while (!this->isOptimized) {
        newProgram = Program();
        this->isOptimized = true;

        for (size_t i = 0; i < oldProgram.body.size(); i++)
        {
            auto statementOrExpression = std::visit([&](const auto& node) -> std::variant<Expression, Statement> {
                using NodeType = std::decay_t<decltype(*node)>;

                if constexpr (std::is_same_v<NodeType, Statement>) {
                    return this->optimizeStatement(*node);
                } else {
                    return this->optimizeExpression(*node);
                }
            }, oldProgram.body.at(i));

            if (std::holds_alternative<Statement>(statementOrExpression)) {
                newProgram.body.push_back(
                    std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
                );
            } else if (std::holds_alternative<Expression>(statementOrExpression)) {
                newProgram.body.push_back(
                    std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
                );
            }
        }

        newProgram.prettyPrint();
        std::cout << std::to_string(count) << "_____________________" << std::endl;

        oldProgram = newProgram;
        count++;
    }

    return newProgram;
};
