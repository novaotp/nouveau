#include <iostream>
#include <variant>
#include <string>
#include "file.hpp"
#include "lexer.hpp"
#include "parser.hpp"

constexpr int SPACE_COUNT = 4;

template <typename NodeType>
void printExpression(const NodeType& n, const size_t indentCount) {
    if constexpr (std::is_same_v<LiteralType, StringLiteral>) {
        std::cout << indent << "StringLiteral: " << literal.value << std::endl;
    } else if constexpr (std::is_same_v<LiteralType, IntLiteral>) {
        std::cout << indent << "IntLiteral: " << literal.value << std::endl;
    } else if constexpr (std::is_same_v<LiteralType, FloatLiteral>) {
        std::cout << indent << "FloatLiteral: " << literal.value << std::endl;
    } else if constexpr (std::is_same_v<LiteralType, BooleanLiteral>) {
        std::cout << indent << "BooleanLiteral: " << (literal.value ? "true" : "false") << std::endl;
    } else if constexpr (std::is_same_v<LiteralType, BooleanLiteral>) {
        std::cout << indent << "NullLiteral" << std::endl;
    } else if constexpr (std::is_same_v<NodeType, BinaryOperation>) {
        std::string indent(indentCount * SPACE_COUNT, ' ');
        std::cout << indent << "BinaryOperation" << std::endl;

        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.lhs);
        std::cout << indent + std::string(SPACE_COUNT, ' ') << "Operator: " << n.op << std::endl;
        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.rhs);
    } else {
        std::string indent(indentCount * SPACE_COUNT, ' ');
        std::cout << indent << "Unknown Expression Type" << std::endl;
    }
}

template <typename NodeType>
void printStatement(const NodeType& n, const size_t indentCount) {
    if constexpr (std::is_same_v<NodeType, VariableDeclaration>) {
        std::string indent(indentCount * SPACE_COUNT, ' ');

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
        std::string indent(indentCount * SPACE_COUNT, ' ');

        std::cout << "Variable Assignment" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << n.identifier << std::endl;

        if (n.value.has_value()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *n.value.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value: null" << std::endl;
        }
    } else {
        std::string indent(indentCount * SPACE_COUNT, ' ');
        std::cout << indent << "Unknown Statement Type" << std::endl;
    }
}

void printProgram(Program& program) {
    for (size_t i = 0; i < program.body.size(); i++) {
        const auto& node = program.body[i];

        std::visit([](const auto& ptr) {
            using PtrType = std::decay_t<decltype(ptr)>;

            if (ptr) {
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
            } else {
                std::cout << "Null Pointer" << std::endl;
            } }, node);
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
