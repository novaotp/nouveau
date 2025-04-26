#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "constant_folding.hpp"

// Not testing variables declarations/assignments because
// they optimize their values, which are expressions

TEST_CASE("Constant folding works correctly") {
    SECTION("Binary operations are optimized correctly") {
        SECTION("Addition between strings -> string") {
            std::string expected = "Hello World";

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(StringLiteral(NodeMetadata(), "Hello")),
                        BinaryOperator::ADDITION,
                        std::make_shared<Expression>(StringLiteral(NodeMetadata(), " World"))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<StringLiteral>(*expression));
            StringLiteral stringLiteral = std::get<StringLiteral>(*expression);

            REQUIRE(stringLiteral.value == expected);
        }

        SECTION("Addition between integers -> int") {
            int expected = 2;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 1)),
                        BinaryOperator::ADDITION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 1))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<IntLiteral>(*expression));
            IntLiteral intLiteral = std::get<IntLiteral>(*expression);

            REQUIRE(intLiteral.value == expected);
        }

        SECTION("Addition between floats -> float") {
            double expected = 2.4;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 1.2F)),
                        BinaryOperator::ADDITION,
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 1.2F))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<FloatLiteral>(*expression));
            FloatLiteral floatLiteral = std::get<FloatLiteral>(*expression);

            REQUIRE_THAT(floatLiteral.value, Catch::Matchers::WithinAbs(expected, 0.0000001));
        }

        SECTION("Addition between an int and a float -> float") {
            double expected = 2.4;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 1)),
                        BinaryOperator::ADDITION,
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 1.4F))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<FloatLiteral>(*expression));
            FloatLiteral floatLiteral = std::get<FloatLiteral>(*expression);

            REQUIRE_THAT(floatLiteral.value, Catch::Matchers::WithinAbs(expected, 0.0000001));
        }

        SECTION("Subtraction between ints -> int") {
            int expected = 3;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 7)),
                        BinaryOperator::SUBTRACTION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 4))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<IntLiteral>(*expression));
            IntLiteral intLiteral = std::get<IntLiteral>(*expression);

            REQUIRE(intLiteral.value == expected);
        }

        SECTION("Subtraction between floats -> float") {
            double expected = 3.5;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 9.2F)),
                        BinaryOperator::SUBTRACTION,
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 5.7F))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<FloatLiteral>(*expression));
            FloatLiteral floatLiteral = std::get<FloatLiteral>(*expression);

            REQUIRE_THAT(floatLiteral.value, Catch::Matchers::WithinAbs(expected, 0.0000001));
        }

        SECTION("Multiplication between a string and an int -> string") {
            std::string expected = "hellohellohellohello";

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(StringLiteral(NodeMetadata(), "hello")),
                        BinaryOperator::MULTIPLICATION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 4))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<StringLiteral>(*expression));
            StringLiteral stringLiteral = std::get<StringLiteral>(*expression);

            REQUIRE(stringLiteral.value == expected);
        }

        SECTION("Multiplication between ints -> int") {
            int expected = 20;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 5)),
                        BinaryOperator::MULTIPLICATION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 4))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<IntLiteral>(*expression));
            IntLiteral intLiteral = std::get<IntLiteral>(*expression);

            REQUIRE(intLiteral.value == expected);
        }

        SECTION("Multiplication between an int and 0 as int -> 0 as int") {
            int expected = 0;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 5)),
                        BinaryOperator::MULTIPLICATION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 0))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<IntLiteral>(*expression));
            IntLiteral intLiteral = std::get<IntLiteral>(*expression);

            REQUIRE(intLiteral.value == expected);
        }

        SECTION("Multiplication between a float and 0 as int -> 0 as int") {
            int expected = 0; // * Because anything times 0 is always 0

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 2.5F)),
                        BinaryOperator::MULTIPLICATION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 0))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<IntLiteral>(*expression));
            IntLiteral intLiteral = std::get<IntLiteral>(*expression);

            REQUIRE_THAT(intLiteral.value, Catch::Matchers::WithinAbs(expected, 0.0000001));
        }

        SECTION("Division between integers -> float") {
            double expected = 2.5;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 5)),
                        BinaryOperator::DIVISION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 2))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<FloatLiteral>(*expression));
            FloatLiteral floatLiteral = std::get<FloatLiteral>(*expression);

            REQUIRE(floatLiteral.value == expected);
        }

        SECTION("Division between floats -> float") {
            double expected = 3;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 7.5F)),
                        BinaryOperator::DIVISION,
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 2.5F))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<FloatLiteral>(*expression));
            FloatLiteral floatLiteral = std::get<FloatLiteral>(*expression);

            REQUIRE_THAT(floatLiteral.value, Catch::Matchers::WithinAbs(expected, 0.0000001));
        }

        SECTION("Division between an int and a float -> float") {
            double expected = 4.1666666;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 10)),
                        BinaryOperator::DIVISION,
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 2.4F))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<FloatLiteral>(*expression));
            FloatLiteral floatLiteral = std::get<FloatLiteral>(*expression);

            REQUIRE_THAT(floatLiteral.value, Catch::Matchers::WithinAbs(expected, 0.0000001));
        }

        SECTION("Division between a float and an int -> float") {
            double expected = 0.5;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(FloatLiteral(NodeMetadata(), 1.5F)),
                        BinaryOperator::DIVISION,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 3))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<FloatLiteral>(*expression));
            FloatLiteral floatLiteral = std::get<FloatLiteral>(*expression);

            REQUIRE_THAT(floatLiteral.value, Catch::Matchers::WithinAbs(expected, 0.0000001));
        }

        SECTION("Modulo between ints -> int") {
            int expected = 1;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 10)),
                        BinaryOperator::MODULO,
                        std::make_shared<Expression>(IntLiteral(NodeMetadata(), 3))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<IntLiteral>(*expression));
            IntLiteral intLiteral = std::get<IntLiteral>(*expression);

            REQUIRE(intLiteral.value == expected);
        }
    }

    SECTION("Logical NOT operations are optimized correctly") {
        SECTION("NOT string, int, float, true -> false") {
            bool expected = false;

            std::vector<Expression> expressions = {
                StringLiteral(NodeMetadata(), "Hello"),
                IntLiteral(NodeMetadata(), 10),
                FloatLiteral(NodeMetadata(), 2.5F),
                BooleanLiteral(NodeMetadata(), true)
            };

            Program program;
            for (const auto expression : expressions) {
                program.body.push_back(
                    std::make_shared<Expression>(
                        LogicalNotOperation(
                            NodeMetadata(),
                            std::make_shared<Expression>(std::move(expression))
                        )
                    )
                );
            }

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 4);

            for (const auto& node : optimizedProgram.body) {
                REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
                std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

                REQUIRE(std::holds_alternative<BooleanLiteral>(*expression));
                BooleanLiteral booleanLiteral = std::get<BooleanLiteral>(*expression);

                REQUIRE(booleanLiteral.value == expected);
            }
        }

        SECTION("NOT false -> true") {
            bool expected = true;

            Program program;
            program.body.push_back(
                std::make_shared<Expression>(
                    LogicalNotOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(BooleanLiteral(NodeMetadata(), false))
                    )
                )
            );

            ConstantFolder constantFolder(program, std::make_shared<Scope>());
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 1);
            auto node = optimizedProgram.body.at(0);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<BooleanLiteral>(*expression));
            BooleanLiteral booleanLiteral = std::get<BooleanLiteral>(*expression);

            REQUIRE(booleanLiteral.value == expected);
        }
    }

    SECTION("Expressions containing identifiers are optimized correctly") {
        SECTION("Identifier for a constant string is replaced") {
            std::string expected = "hello world";

            auto variableDecl = VariableDeclaration(
                                    NodeMetadata(),
                                    false,
                                    std::make_shared<StringType>(),
                                    "start",
                                    std::make_shared<Expression>(StringLiteral(NodeMetadata(), "hello"))
                                );

            Program program;
            program.body.push_back(std::make_shared<Statement>(variableDecl));
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(Identifier(NodeMetadata(), "start")),
                        BinaryOperator::ADDITION,
                        std::make_shared<Expression>(StringLiteral(NodeMetadata(), " world"))
                    )
                )
            );

            Scope scope;
            scope.addSymbol("start", std::make_shared<VariableDeclaration>(variableDecl));

            ConstantFolder constantFolder(program, std::make_shared<Scope>(scope));
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 2);
            auto node = optimizedProgram.body.at(1);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<StringLiteral>(*expression));
            StringLiteral stringLiteral = std::get<StringLiteral>(*expression);

            REQUIRE(stringLiteral.value == expected);
        }

        SECTION("Identifier for a mutable string is NOT replaced") {
            auto expected = BinaryOperation(
                                NodeMetadata(),
                                std::make_shared<Expression>(Identifier(NodeMetadata(), "start")),
                                BinaryOperator::ADDITION,
                                std::make_shared<Expression>(StringLiteral(NodeMetadata(), " world"))
                            );

            auto variableDecl = VariableDeclaration(
                                    NodeMetadata(),
                                    true,
                                    std::make_shared<StringType>(),
                                    "start",
                                    std::make_shared<Expression>(StringLiteral(NodeMetadata(), "hello"))
                                );

            Program program;
            program.body.push_back(std::make_shared<Statement>(variableDecl));
            program.body.push_back(
                std::make_shared<Expression>(
                    BinaryOperation(
                        NodeMetadata(),
                        std::make_shared<Expression>(Identifier(NodeMetadata(), "start")),
                        BinaryOperator::ADDITION,
                        std::make_shared<Expression>(StringLiteral(NodeMetadata(), " world"))
                    )
                )
            );

            Scope scope;
            scope.addSymbol("start", std::make_shared<VariableDeclaration>(variableDecl));

            ConstantFolder constantFolder(program, std::make_shared<Scope>(scope));
            auto optimizedProgram = constantFolder.optimize();

            REQUIRE(optimizedProgram.body.size() == 2);
            auto node = optimizedProgram.body.at(1);

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(node));
            std::shared_ptr<Expression> expression = std::get<std::shared_ptr<Expression>>(node);

            REQUIRE(std::holds_alternative<BinaryOperation>(*expression));
            BinaryOperation binaryOperation = std::get<BinaryOperation>(*expression);

            REQUIRE(std::holds_alternative<Identifier>(*binaryOperation.lhs));
            Identifier identifier = std::get<Identifier>(*binaryOperation.lhs);

            REQUIRE(identifier.name == "start");

            REQUIRE(binaryOperation.op == BinaryOperator::ADDITION);

            REQUIRE(std::holds_alternative<StringLiteral>(*binaryOperation.rhs));
            StringLiteral stringLiteral = std::get<StringLiteral>(*binaryOperation.rhs);

            REQUIRE(stringLiteral.value == " world");
        }
    }
}
