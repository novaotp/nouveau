#include <iostream>
#include <variant>
#include <string>
#include "file.hpp"
#include "lexer.hpp"
#include "parser.hpp"

constexpr int SPACE_COUNT = 4;

template <typename NodeType>
void printExpression(const NodeType& n, const size_t indentCount) {
    std::string indent(indentCount * SPACE_COUNT, ' ');

    if constexpr (std::is_same_v<NodeType, StringLiteral>) {
        std::cout << indent << "StringLiteral: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, IntLiteral>) {
        std::cout << indent << "IntLiteral: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, FloatLiteral>) {
        std::cout << indent << "FloatLiteral: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, BooleanLiteral>) {
        std::cout << indent << "BooleanLiteral: " << (n.value ? "true" : "false") << std::endl;
    } else if constexpr (std::is_same_v<NodeType, NullLiteral>) {
        std::cout << indent << "NullLiteral" << std::endl;
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
    } else {
        std::cout << indent << "Unknown Statement Type" << std::endl;
    }
}

void printProgram(Program& program) {
    for (size_t i = 0; i < program.body.size(); i++) {
        const auto& node = program.body[i];

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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Please provide the path to the source file." << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::string contents = readFile(filePath);

    Lexer lexer = Lexer(contents);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser = Parser(tokens);
    Program program = parser.parse();

    printProgram(program);

    return 0;
}
