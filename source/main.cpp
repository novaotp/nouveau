#include <iostream>
#include <variant>
#include <string>
#include "file.hpp"
#include "lexer.hpp"
#include "parser.hpp"

template <typename NodeType>
void printLiteralOrOperation(const NodeType &n)
{
    if constexpr (std::is_same_v<NodeType, Literal>)
    {
        std::visit([](const auto &literal)
                   {
                       using LiteralType = std::decay_t<decltype(literal)>;

                       if constexpr (std::is_same_v<LiteralType, StringLiteral>)
                       {
                           std::cout << "StringLiteral: " << literal.value << std::endl;
                       }
                       else if constexpr (std::is_same_v<LiteralType, IntLiteral>)
                       {
                           std::cout << "IntLiteral: " << literal.value << std::endl;
                       }
                       else if constexpr (std::is_same_v<LiteralType, FloatLiteral>)
                       {
                           std::cout << "FloatLiteral: " << literal.value << std::endl;
                       }
                       else if constexpr (std::is_same_v<LiteralType, BooleanLiteral>)
                       {
                           std::cout << "BooleanLiteral: " << (literal.value ? "true" : "false") << std::endl;
                       } },
                   n);
    }
    else if constexpr (std::is_same_v<NodeType, ArithmeticOperation>)
    {
        std::cout << "ArithmeticOperation: " << n.op << std::endl;
    }
    else
    {
        std::cout << "Unknown Literal or Operation Type" << std::endl;
    }
}

void printProgram(Program &program)
{
    for (size_t i = 0; i < program.body.size(); i++)
    {
        const auto &node = program.body[i];

        std::visit([](const auto &ptr)
                   {
            using PtrType = std::decay_t<decltype(ptr)>;

            if (ptr) {
                using NodeType = std::decay_t<decltype(*ptr)>;

                if constexpr (std::is_same_v<NodeType, Expression>) {
                    std::visit([](const auto &expr) {
                        using ExprType = std::decay_t<decltype(expr)>;

                        if constexpr (std::is_same_v<ExprType, Literal>) {
                            std::visit([](const auto &literal) {
                                using LiteralType = std::decay_t<decltype(literal)>;

                                if constexpr (std::is_same_v<LiteralType, StringLiteral>) {
                                    std::cout << "StringLiteral: " << literal.value << std::endl;
                                } else if constexpr (std::is_same_v<LiteralType, IntLiteral>) {
                                    std::cout << "IntLiteral: " << literal.value << std::endl;
                                } else if constexpr (std::is_same_v<LiteralType, FloatLiteral>) {
                                    std::cout << "FloatLiteral: " << literal.value << std::endl;
                                } else if constexpr (std::is_same_v<LiteralType, BooleanLiteral>) {
                                    std::cout << "BooleanLiteral: " << (literal.value ? "true" : "false") << std::endl;
                                }
                            }, expr);
                        } else if constexpr (std::is_same_v<ExprType, ArithmeticOperation>) {
                            std::cout << "ArithmeticOperation: " << expr.op << std::endl;
                            if (expr.lhs) {
                                std::cout << "LHS: ";
                                std::visit([](const auto &lhs) { printLiteralOrOperation(lhs); }, *expr.lhs);
                            }
                            if (expr.rhs) {
                                std::cout << "RHS: ";
                                std::visit([](const auto &rhs) { printLiteralOrOperation(rhs); }, *expr.rhs);
                            }
                        }
                    }, *ptr);
                }
                else if constexpr (std::is_same_v<NodeType, Statement>) {
                    // Placeholder for Statement handling, currently left empty
                }
                else {
                    std::cout << "Unknown Node Type" << std::endl;
                }
            } else {
                std::cout << "Null Pointer" << std::endl;
            } }, node);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
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
