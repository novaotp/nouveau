#include <iostream>
#include "constant_folding.hpp"

ConstantFolder::ConstantFolder(Program program) : program(program) {};
ConstantFolder::~ConstantFolder() {};

void ConstantFolder::optimizeVariableDeclaration(VariableDeclaration node) {
    if (node.value.has_value()) {
        std::visit([&](const auto& expression) {
            using ExpressionType = std::decay_t<decltype(expression)>;

            if constexpr (std::is_same_v<ExpressionType, BinaryOperation>) {
                std::visit([&](auto&& lhs, auto&& rhs) {
                    using LType = std::decay_t<decltype(lhs)>;
                    using RType = std::decay_t<decltype(rhs)>;

                    if constexpr (
                        (std::is_same_v<LType, IntLiteral> || std::is_same_v<LType, FloatLiteral>) &&
                        (std::is_same_v<RType, IntLiteral> || std::is_same_v<RType, FloatLiteral>)
                    ) {
                        std::optional<Expression> expr;
                        if (
                        (expression.op == "*" && lhs.value != 0 && rhs.value != 0) &&
                        (expression.op == "/" || std::is_same_v<LType, FloatLiteral> || std::is_same_v<RType, FloatLiteral>)
                        ) {
                            float value;
                            if (expression.op == "+") {
                                value = lhs.value + rhs.value;
                            } else if (expression.op == "-") {
                                value = lhs.value - rhs.value;
                            } else if (expression.op == "*") {
                                value = lhs.value * rhs.value;
                            } else if (expression.op == "/") {
                                value = lhs.value / rhs.value;
                            } /* else if (expression.op == "%") {
                                value = lhs.value % rhs.value;
                            } */ else {
                                std::cerr << "Unknown op in constant folding" << std::endl;
                            }

                            expr = FloatLiteral(expression.metadata, value);
                        } else {
                            int value;
                            if (expression.op == "+") {
                                value = lhs.value + rhs.value;
                            } else if (expression.op == "-") {
                                value = lhs.value - rhs.value;
                            } else if (expression.op == "*") {
                                value = lhs.value * rhs.value;
                            } else if (expression.op == "/") {
                                value = lhs.value / rhs.value;
                            } /* else if (expression.op == "%") {
                                value = lhs.value % rhs.value;
                            } */ else {
                                std::cerr << "Unknown op in constant folding" << std::endl;
                            }

                            expr = IntLiteral(expression.metadata, value);
                        }

                        VariableDeclaration variableDeclaration(node.metadata, node.isMutable, node.type, node.identifier, std::make_shared<Expression>(expr.value()));

                        this->newProgram.body.push_back(std::make_shared<Statement>(std::move(variableDeclaration)));
                    } else if (
                    (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, StringLiteral> && expression.op == "+") ||
                        (((std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, IntLiteral>) ||
                    (std::is_same_v<LType, IntLiteral> && std::is_same_v<RType, StringLiteral>)) && expression.op == "*")
                    ) {
                        // * The conditions are constexpr so that .value if inferred correctly
                        // * Otherwise, I get C2039 errors

                        std::optional<Expression> expr;
                        if constexpr (std::is_same_v<LType, StringLiteral> && std::is_same_v<RType, StringLiteral>) {
                            expr = StringLiteral(expression.metadata, lhs.value + rhs.value);
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

                            expr = StringLiteral(expression.metadata, buffer);
                        }

                        VariableDeclaration variableDeclaration(node.metadata, node.isMutable, node.type, node.identifier, std::make_shared<Expression>(expr.value()));

                        this->newProgram.body.push_back(std::make_shared<Statement>(std::move(variableDeclaration)));
                    }
                }, *expression.lhs, *expression.rhs);
            }
        }, *(node.value.value()));
    }
};

void ConstantFolder::optimizeStatement(Statement node) {
    std::visit([&](const auto& statement) {
        using NodeType = std::decay_t<decltype(statement)>;

        if constexpr (std::is_same_v<NodeType, VariableDeclaration>) {
            this->optimizeVariableDeclaration(statement);
        }
    }, node);
}

Program ConstantFolder::optimize() {
    for (size_t i = 0; i < this->program.body.size(); i++)
    {
        std::visit([&](const auto& node) {
            using NodeType = std::decay_t<decltype(*node)>;

            if constexpr (std::is_same_v<NodeType, Statement>) {
                this->optimizeStatement(*node);
            }
        }, this->program.body.at(i));
    }

    return this->newProgram;
};
