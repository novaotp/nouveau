#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include "token.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semer.hpp"

std::string fakePath = "fake-path.nv";

TEST_CASE("Semer works correctly", "[semer]") {
    SECTION("Variable declarations are analyzed properly") {
        SECTION("Variable declarations without initial values are flagged as SEMANTIC ERROR because 'null' values are not supported yet") {
            // * 'is_valid;' added to prevent unused variable warning

            std::vector<std::string> sourceCodes = {
                "bool is_valid; is_valid;",
                "mut bool is_valid; is_valid;",
            };

            for (const auto sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program, fakePath);
                auto [errors, scope] = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SEMANTIC_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }

        SECTION("Variable declarations with a value of the wrong type are flagged as TYPE ERROR") {
            // * 'is_valid;' added to prevent unused variable warning

            std::vector<std::string> sourceCodes = {
                "bool is_valid = 69; is_valid;",
                "int is_valid = \"test\"; is_valid;",
                "float is_valid = true; is_valid;",
                "string is_valid = 3.14; is_valid;"
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program, fakePath);
                auto [errors, scope] = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::TYPE_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }
    }

    SECTION("Variable assignments are analyzed properly") {
        SECTION("Variable assignments that are not declared beforehand are flagged as SYNTAX ERROR") {
            std::vector<std::string> sourceCodes = {
                "message = \"Hello, World !\";",
                "message = 69;"
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program, fakePath);
                auto [errors, scope] = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SYNTAX_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }

        SECTION("Variable assignments with a value of the wrong type are flagged as TYPE ERROR") {
            // * 'message;' added to prevent unused variable warning

            std::vector<std::string> sourceCodes = {
                "mut string message = \"Hello, World!\"; message = 69; message;",
                "mut string message = \"Hello, World!\"; message = true; message;",
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program, fakePath);
                auto [errors, scope] = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::TYPE_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }

        SECTION("Assigning to a constant variable is flagged as SEMANTIC ERROR") {
            // * 'counter;' and 'message;' added to prevent unused variable warning

            std::vector<std::string> sourceCodes = {
                "int counter = 0; counter = 69; counter;",
                "string message = \"Hello, World!\"; message = \"Goodbye, World!\"; message;",
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program, fakePath);
                auto [errors, scope] = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SEMANTIC_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }
    }

    SECTION("Expressions are analyzed properly") {
        SECTION("Identifiers that are not declared beforehand are flagged as SYNTAX ERROR") {
            std::string sourceCode = "message = \"Hello, World!\"";
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            Semer semer(sourceCode, program, fakePath);
            auto [errors, scope] = semer.analyze();

            REQUIRE(errors.size() == 1);

            const SemerError& error = errors.at(0);

            REQUIRE(error.type == SemerErrorType::SYNTAX_ERROR);
            REQUIRE(error.level == SemerErrorLevel::ERROR);
        }

        SECTION("Binary operations with invalid operands are flagged as TYPE ERROR") {
            // * 'message;' added to prevent unused variable warning

            std::vector<std::string> sourceCodes = {
                "string message = \"Hello\" + 5; message;",
                "string message = \"Hello\" + true; message;",
                "string message = true / 3.14; message;",
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program, fakePath);
                auto [errors, scope] = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SYNTAX_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }
    }

    SECTION("Symbol table works correctly") {
        SECTION("Unused variables are flagged as SEMANTIC WARNING") {
            std::vector<std::string> sourceCodes = {
                "string message = \"Hello\";",
                "int count = 69;"
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program, fakePath);
                auto [errors, scope] = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SEMANTIC_ERROR);
                REQUIRE(error.level == SemerErrorLevel::WARNING);
            }
        }
    }
}
