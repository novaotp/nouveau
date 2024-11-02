#include <iostream>
#include "ast.hpp"

constexpr int SPACE_COUNT = 4;

template <typename NodeType>
void printExpression(const NodeType& n, const size_t indentCount) {
    std::string indent(indentCount * SPACE_COUNT, ' ');

    if constexpr (std::is_same_v<NodeType, StringLiteral>) {
        std::cout << indent << "String: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, IntLiteral>) {
        std::cout << indent << "Int: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, FloatLiteral>) {
        std::cout << indent << "Float: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, BooleanLiteral>) {
        std::cout << indent << "Boolean: " << (n.value ? "true" : "false") << std::endl;
    } else if constexpr (std::is_same_v<NodeType, NullLiteral>) {
        std::cout << indent << "Null" << std::endl;
    } else if constexpr (std::is_same_v<NodeType, Identifier>) {
        std::cout << indent << "Identifier: " << n.name << std::endl;
    } else if constexpr (std::is_same_v<NodeType, BinaryOperation>) {
        std::cout << indent << "BinaryOperation" << std::endl;

        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.lhs);
        std::cout << indent + std::string(SPACE_COUNT, ' ') << "Operator: " << n.op << std::endl;
        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.rhs);
    } else if constexpr (std::is_same_v<NodeType, LogicalNotOperation>) {
        std::cout << indent << "NotOperation" << std::endl;
        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.expression);
    } else if constexpr (std::is_same_v<NodeType, Vector>) {
        std::cout << indent << "Vector" << std::endl;

        for (const auto& expr : n.values) {
            std::visit([&indentCount](const auto& element) {
                printExpression(element, indentCount + 1);
            }, *expr);
        }
    } else if constexpr (std::is_same_v<NodeType, Function>) {
        std::cout << indent << "Function" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Name: " << n.name << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Return Type: " << n.returnType << std::endl;

        if (n.parameters.empty()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Parameters: None" << std::endl;
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Parameters" << std::endl;

            for (const auto& param : n.parameters) {
                printStatement(*param, indentCount + 2);
            }
        }

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Body" << std::endl;
        for (const auto& element : n.body) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Body" << std::endl;
                }
            }, element);
        }
    } else if constexpr (std::is_same_v<NodeType, FunctionCall>) {
        std::cout << indent << "Function Call" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << n.identifier << std::endl;

        if (n.arguments.empty()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Arguments: None" << std::endl;
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Arguments" << std::endl;

            for (const auto& arg : n.arguments) {
                std::visit([&indentCount](const auto& expr) {
                    printExpression(expr, indentCount + 2);
                }, *arg);
            }
        }
    } else {
        std::cout << indent << "Unknown Expression Type" << std::endl;
    }
}

template <typename NodeType>
void printStatement(const NodeType& n, const size_t indentCount) {
    std::string indent(indentCount * SPACE_COUNT, ' ');

    if constexpr (std::is_same_v<NodeType, VariableDeclaration>) {
        std::cout << "Variable Declaration" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "IsMutable: " << (n.isMutable ? "true" : "false") << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Type: " << n.type << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << n.identifier << std::endl;

        if (n.value.has_value()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *n.value.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value: null" << std::endl;
        }
    } else if constexpr (std::is_same_v<NodeType, VariableAssignment>) {

        std::cout << indent << "Variable Assignment" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << n.identifier << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Operator: " << n.op << std::endl;

        if (n.value.has_value()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *n.value.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value: null" << std::endl;
        }
    } else if constexpr (std::is_same_v<NodeType, IfStatement>) {
        std::cout << indent << "If Statement" << std::endl;

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Condition:" << std::endl;
        std::visit([&indentCount](const auto& condition) {
            printExpression(condition, indentCount + 2);
        }, *n.condition);

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Then Block:" << std::endl;
        for (const auto& element : n.thenBlock) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Then Block" << std::endl;
                }
            }, element);
        }

        for (const auto& elseif : n.elseifClauses) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Else If:" << std::endl;
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "Condition:" << std::endl;
            std::visit([&indentCount](const auto& condition) {
                printExpression(condition, indentCount + 3);
            }, *elseif.first);

            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "Block:" << std::endl;
            for (const auto& element : elseif.second) {
                std::visit([&indentCount](const auto& item) {
                    using ItemType = std::decay_t<decltype(*item)>;

                    if constexpr (std::is_same_v<ItemType, Statement>) {
                        std::visit([&indentCount](const auto& expr) {
                            printStatement(expr, indentCount + 3);
                        }, *item);
                    } else if constexpr (std::is_same_v<ItemType, Expression>) {
                        std::visit([&indentCount](const auto& expr) {
                            printExpression(expr, indentCount + 3);
                        }, *item);
                    } else {
                        std::cout << "Unknown Type in Then Block" << std::endl;
                    }
                }, element);
            }
        }

        if (!n.elseBlock.empty()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Else Block:" << std::endl;
            for (const auto& element : n.elseBlock) {
                std::visit([&indentCount](const auto& item) {
                    using ItemType = std::decay_t<decltype(*item)>;

                    if constexpr (std::is_same_v<ItemType, Statement>) {
                        std::visit([&indentCount](const auto& expr) {
                            printStatement(expr, indentCount + 2);
                        }, *item);
                    } else if constexpr (std::is_same_v<ItemType, Expression>) {
                        std::visit([&indentCount](const auto& expr) {
                            printExpression(expr, indentCount + 2);
                        }, *item);
                    } else {
                        std::cout << "Unknown Type in Then Block" << std::endl;
                    }
                }, element);
            }
        }
    } else if constexpr (std::is_same_v<NodeType, WhileStatement>) {
        std::cout << indent << "While Statement" << std::endl;

        /**
         * CONDITION
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Condition" << std::endl;
        std::visit([&indentCount](const auto& condition) {
            printStatement(condition, indentCount + 2);
        }, *n.condition);

        /**
         * BLOCK
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Block" << std::endl;
        for (const auto& element : n.block) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Block" << std::endl;
                }
            }, element);
        }
    } else if constexpr (std::is_same_v<NodeType, ForStatement>) {
        std::cout << indent << "For Statement" << std::endl;

        /**
         * INITIALIZATION
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Initialization" << std::endl;
        if (n.initialization.has_value()) {
            std::visit([&indentCount](const auto& stmt) {
                printStatement(stmt, indentCount + 2);
            }, *n.initialization.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "None provided" << std::endl;
        }

        /**
         * CONDITION
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Condition" << std::endl;
        if (n.condition.has_value()) {
            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *n.condition.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "None provided" << std::endl;
        }

        /**
         * UPDATE
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Update" << std::endl;
        if (n.update.has_value()) {
            std::visit([&indentCount](const auto& stmt) {
                printStatement(stmt, indentCount + 2);
            }, *n.update.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "None provided" << std::endl;
        }

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Block" << std::endl;
        for (const auto& element : n.block) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Block" << std::endl;
                }
            }, element);
        }
    } else if constexpr (std::is_same_v<NodeType, BreakStatement>) {
        std::cout << indent << "Break" << std::endl;
    } else if constexpr (std::is_same_v<NodeType, ContinueStatement>) {
        std::cout << indent << "Continue" << std::endl;
    } else if constexpr (std::is_same_v<NodeType, ReturnStatement>) {
        std::cout << indent << "Return" << std::endl;

        if (n.expression.has_value()) {
            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 1);
            }, *n.expression.value());
        }
    } else {
        std::cout << indent << "Unknown Statement Type" << std::endl;
    }
}

void Program::prettyPrint() {
    for (size_t i = 0; i < this->body.size(); i++) {
        const auto& node = this->body[i];

        std::visit([](const auto& ptr) {
            using NodeType = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<NodeType, Expression>) {
                std::visit([](const auto& expr) {
                    printExpression(expr, 0);
                }, *ptr);
            } else if constexpr (std::is_same_v<NodeType, Statement>) {
                std::visit([](const auto& expr) {
                    printStatement(expr, 0);
                }, *ptr);
            } else {
                std::cout << "Unknown Node Type" << std::endl;
            }
        }, node);
    }
}
