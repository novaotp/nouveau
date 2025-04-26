#include <iostream>
#include "constant_folding.hpp"

bool isComparisonOperator(BinaryOperator op) {
    return
    op == BinaryOperator::GREATER_OR_EQUAL ||
    op == BinaryOperator::GREATER_THAN ||
    op == BinaryOperator::LESS_OR_EQUAL||
    op == BinaryOperator::LESS_THAN ||
    op == BinaryOperator::EQUAL ||
    op == BinaryOperator::NOT_EQUAL;
};

bool isArithmeticOperator(BinaryOperator op) {
    return
    op == BinaryOperator::ADDITION ||
    op == BinaryOperator::SUBTRACTION ||
    op == BinaryOperator::MULTIPLICATION ||
    op == BinaryOperator::DIVISION ||
    op == BinaryOperator::MODULO;
};

bool isLogicalOperator(BinaryOperator op) {
    return op == BinaryOperator::AND || op == BinaryOperator::OR;
};

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
        } else {
            return LogicalNotOperation(
                node.metadata,
                std::make_shared<Expression>(this->optimizeExpression(expression))
            );
        }
    }, *node.expression);
};

Expression ConstantFolder::optimizeComparisonBinaryOperation(BinaryOperation node) {
    return std::visit([&](auto&& lhs, auto&& rhs) -> Expression {
        using LType = std::decay_t<decltype(lhs)>;
        using RType = std::decay_t<decltype(rhs)>;

        if constexpr (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, StringLiteral>) {
            this->isOptimized = false;

            switch (node.op) {
            // Greater or less are always going to be false, so only the equal matters
            case BinaryOperator::GREATER_OR_EQUAL:
            case BinaryOperator::LESS_OR_EQUAL:
            case BinaryOperator::EQUAL:
                return BooleanLiteral(node.metadata, lhs.value == rhs.value);

            case BinaryOperator::NOT_EQUAL:
                return BooleanLiteral(node.metadata, lhs.value != rhs.value);

            // No string can compare to another one
            case BinaryOperator::GREATER_THAN:
            case BinaryOperator::LESS_THAN:
                return BooleanLiteral(node.metadata, false);

            default:
                throw std::runtime_error("Expected comparison operator, found " + node.op);
            }
        } else if constexpr (
            (std::is_same_v<LType, IntLiteral> || std::is_same_v<LType, FloatLiteral>) &&
            (std::is_same_v<RType, IntLiteral> || std::is_same_v<RType, FloatLiteral>)
        ) {
            this->isOptimized = false;

            switch (node.op) {
            case BinaryOperator::EQUAL:
                return BooleanLiteral(node.metadata, lhs.value == rhs.value);
            case BinaryOperator::NOT_EQUAL:
                return BooleanLiteral(node.metadata, lhs.value != rhs.value);
            case BinaryOperator::GREATER_OR_EQUAL:
                return BooleanLiteral(node.metadata, lhs.value >= rhs.value);
            case BinaryOperator::GREATER_THAN:
                return BooleanLiteral(node.metadata, lhs.value > rhs.value);
            case BinaryOperator::LESS_OR_EQUAL:
                return BooleanLiteral(node.metadata, lhs.value <= rhs.value);
            case BinaryOperator::LESS_THAN:
                return BooleanLiteral(node.metadata, lhs.value < rhs.value);
            default:
                throw std::runtime_error("Expected comparison operator, found " + node.op);
            }
        } else if constexpr (std::is_same_v<LType, BooleanLiteral> && std::is_same_v<RType, BooleanLiteral>) {
            this->isOptimized = false;

            switch (node.op) {
            // Greater or less are always going to be false, so only the equal matters
            case BinaryOperator::EQUAL:
            case BinaryOperator::GREATER_OR_EQUAL:
            case BinaryOperator::LESS_OR_EQUAL:
                return BooleanLiteral(node.metadata, lhs.value == rhs.value);

            case BinaryOperator::NOT_EQUAL:
                return BooleanLiteral(node.metadata, lhs.value != rhs.value);

            // No boolean can compare to another one
            case BinaryOperator::GREATER_THAN:
            case BinaryOperator::LESS_THAN:
                return BooleanLiteral(node.metadata, false);

            default:
                throw std::runtime_error("Expected comparison operator, found " + node.op);
            }
        } else {
            return BinaryOperation(
                       node.metadata,
                       std::make_shared<Expression>(this->optimizeExpression(lhs)),
                       node.op,
                       std::make_shared<Expression>(this->optimizeExpression(rhs))
                   );
        }
    }, *node.lhs, *node.rhs);
};

Expression ConstantFolder::optimizeArithmeticBinaryOperation(BinaryOperation node) {
    return std::visit([&](auto&& lhs, auto&& rhs) -> Expression {
        using LType = std::decay_t<decltype(lhs)>;
        using RType = std::decay_t<decltype(rhs)>;

        if constexpr (
            (std::is_same_v<LType, IntLiteral> || std::is_same_v<LType, FloatLiteral>) &&
            (std::is_same_v<RType, IntLiteral> || std::is_same_v<RType, FloatLiteral>)
        ) {
            std::optional<Expression> expr;
            if (
                node.op != BinaryOperator::MODULO && (!(node.op == BinaryOperator::MULTIPLICATION && (lhs.value == 0 || rhs.value == 0))) &&
                (node.op == BinaryOperator::DIVISION || std::is_same_v<LType, FloatLiteral> || std::is_same_v<RType, FloatLiteral>)
            ) {
                float value;
                if (node.op == BinaryOperator::ADDITION) {
                    value = float(lhs.value) + float(rhs.value);
                } else if (node.op == BinaryOperator::SUBTRACTION) {
                    value = float(lhs.value) - float(rhs.value);
                } else if (node.op == BinaryOperator::MULTIPLICATION) {
                    value = float(lhs.value) * float(rhs.value);
                } else if (node.op == BinaryOperator::DIVISION) {
                    value = float(lhs.value) / float(rhs.value);
                } else {
                    std::cerr << "Unknown op in constant folding" << std::endl;
                    return node;
                }

                expr = FloatLiteral(node.metadata, value);
            } else {
                // * No division because divisions always end up as floats

                int value;
                if (node.op == BinaryOperator::ADDITION) {
                    value = int(lhs.value) + int(rhs.value);
                } else if (node.op == BinaryOperator::SUBTRACTION) {
                    value = int(lhs.value) - int(rhs.value);
                } else if (node.op == BinaryOperator::MULTIPLICATION) {
                    value = int(lhs.value) * int(rhs.value);
                } else if (node.op == BinaryOperator::MODULO) {
                    value = int(lhs.value) % int(rhs.value);
                } else {
                    std::cerr << "Unknown op in constant folding" << std::endl;
                    return node;
                }

                expr = IntLiteral(node.metadata, value);
            }

            this->isOptimized = false;
            return expr.value();
        } else if (
        (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, StringLiteral> && node.op == BinaryOperator::ADDITION) ||
            (((std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, IntLiteral>) ||
        (std::is_same_v<LType, IntLiteral> && std::is_same_v<RType, StringLiteral>)) && node.op == BinaryOperator::MULTIPLICATION)
        ) {
            // * The conditions are constexpr so that .value if inferred correctly
            // * Otherwise, I get C2039 errors

            std::optional<Expression> expr;
            if constexpr (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, StringLiteral>) {
                expr = StringLiteral(node.metadata, lhs.value + rhs.value);
            } else {
                std::string str;
                size_t max;
                if constexpr (std::is_same_v<LType, IntLiteral> && std::is_same_v<RType, StringLiteral>) {
                    max = size_t(lhs.value);
                    str = rhs.value;
                } else if constexpr (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, IntLiteral>) {
                    max = size_t(rhs.value);
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
        } else {
            return BinaryOperation(
                       node.metadata,
                       std::make_shared<Expression>(this->optimizeExpression(lhs)),
                       node.op,
                       std::make_shared<Expression>(this->optimizeExpression(rhs))
                   );
        }
    }, *node.lhs, *node.rhs);
};

Expression ConstantFolder::optimizeLogicalBinaryOperation(BinaryOperation node) {
    return std::visit([&](auto&& lhs, auto&& rhs) -> Expression {
        using LType = std::decay_t<decltype(lhs)>;
        using RType = std::decay_t<decltype(rhs)>;

        if constexpr (
            (
                std::is_same_v<LType, StringLiteral> ||
                std::is_same_v<LType, IntLiteral> ||
                std::is_same_v<LType, FloatLiteral> ||
                std::is_same_v<LType, BooleanLiteral>
            ) &&
            (
                std::is_same_v<RType, StringLiteral> ||
                std::is_same_v<RType, IntLiteral> ||
                std::is_same_v<RType, FloatLiteral> ||
                std::is_same_v<RType, BooleanLiteral>
            )
        ) {
            this->isOptimized = false;

            if (node.op == BinaryOperator::AND) {
                if constexpr (std::is_same_v<LType, BooleanLiteral>) {
                    // false AND something -> false
                    if (!lhs.value) {
                        return BooleanLiteral(node.metadata, false);
                    }
                }

                if constexpr (std::is_same_v<RType, BooleanLiteral>) {
                    // something AND false -> false
                    if (!rhs.value) {
                        return BooleanLiteral(node.metadata, false);
                    }
                }

                // All values except null and false are truthy
                return BooleanLiteral(node.metadata, true);
            } else {
                if constexpr (std::is_same_v<LType, BooleanLiteral> && std::is_same_v<RType, BooleanLiteral>) {
                    if (!lhs.value && !rhs.value) {
                        return BooleanLiteral(node.metadata, false);
                    }
                }

                // All values except null and false are truthy
                return BooleanLiteral(node.metadata, true);
            }
        } else {
            return BinaryOperation(
                       node.metadata,
                       std::make_shared<Expression>(this->optimizeExpression(lhs)),
                       node.op,
                       std::make_shared<Expression>(this->optimizeExpression(rhs))
                   );
        }
    }, *node.lhs, *node.rhs);
};

Expression ConstantFolder::optimizeBinaryOperation(BinaryOperation node) {
    if (isComparisonOperator(node.op)) {
        return this->optimizeComparisonBinaryOperation(node);
    } else if (isArithmeticOperator(node.op)) {
        return this->optimizeArithmeticBinaryOperation(node);
    } else if (isLogicalOperator(node.op)) {
        return this->optimizeLogicalBinaryOperation(node);
    } else {
        std::cout << "Cannot fold binary operation because of unknown binary operator : " << node.op << std::endl;
        return node;
    }
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
        using StatementType = std::decay_t<decltype(statement)>;

        if constexpr (std::is_same_v<StatementType, VariableDeclaration>) {
            return this->optimizeVariableDeclaration(statement);
        } else if constexpr (std::is_same_v<StatementType, VariableAssignment>) {
            return this->optimizeVariableAssignment(statement);
        } else {
            return statement;
        }
    }, node);
}

Program ConstantFolder::optimize() {
    Program oldProgram = this->program;
    Program newProgram = Program();

    while (!this->isOptimized) {
        newProgram = Program();
        this->isOptimized = true;

        for (const auto& item : oldProgram.body) {
            std::visit([&](const auto& node) {
                using StatementOrExpressionType = std::decay_t<decltype(*node)>;

                if constexpr (std::is_same_v<StatementOrExpressionType, Statement>) {
                    newProgram.body.push_back(
                        std::make_shared<Statement>(
                            this->optimizeStatement(*node)
                        )
                    );
                } else {
                    newProgram.body.push_back(
                        std::make_shared<Expression>(
                            this->optimizeExpression(*node)
                        )
                    );
                }
            }, item);
        }

        oldProgram = newProgram;
    }

    return newProgram;
};
