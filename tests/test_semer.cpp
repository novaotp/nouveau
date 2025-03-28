#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include "token.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semer.hpp"

TEST_CASE("Semer works correctly", "[semer]") {
    SECTION("Variable declarations are analyzed properly") {
        SECTION("Variable declarations without initial values are flagged as SEMANTIC ERROR because 'null' values are not supported yet") {
            std::vector<std::string> sourceCodes = {
                "bool is_valid;",
                "mut bool is_valid;",
            };

            for (const auto sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program);
                const std::vector<SemerError>& errors = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SEMANTIC_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }

        SECTION("Variable declarations with a value of the wrong type are flagged as TYPE ERROR") {
            std::vector<std::string> sourceCodes = {
                "bool is_valid = 69;",
                "int is_valid = \"test\"",
                "float is_valid = true;",
                "string is_valid = 3.14;"
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program);
                const std::vector<SemerError>& errors = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::TYPE_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }
    }

    SECTION("Statements are analyzed properly") {
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

                Semer semer(sourceCode, program);
                const std::vector<SemerError>& errors = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SYNTAX_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }

        SECTION("Variable assignments with a value of the wrong type are flagged as TYPE ERROR") {
            std::vector<std::string> sourceCodes = {
                "string message = \"Hello, World!\"; message = 69;",
                "string message = \"Hello, World!\"; message = true;",
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program);
                const std::vector<SemerError>& errors = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::TYPE_ERROR);
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

            Semer semer(sourceCode, program);
            const std::vector<SemerError>& errors = semer.analyze();

            REQUIRE(errors.size() == 1);

            const SemerError& error = errors.at(0);

            REQUIRE(error.type == SemerErrorType::SYNTAX_ERROR);
            REQUIRE(error.level == SemerErrorLevel::ERROR);
        }

        SECTION("Binary operations with invalid operands are flagged as TYPE ERROR") {
            std::vector<std::string> sourceCodes = {
                "string message = \"Hello\" + 5;",
                "string message = \"Hello\" + true;",
                "string message = true / 3.14;",
            };

            for (const std::string sourceCode : sourceCodes) {
                Lexer lexer = Lexer(sourceCode);
                std::vector<Token> tokens = lexer.tokenize();

                Parser parser(sourceCode, tokens);
                Program program = parser.parse();

                Semer semer(sourceCode, program);
                const std::vector<SemerError>& errors = semer.analyze();

                REQUIRE(errors.size() == 1);

                const SemerError& error = errors.at(0);

                REQUIRE(error.type == SemerErrorType::SYNTAX_ERROR);
                REQUIRE(error.level == SemerErrorLevel::ERROR);
            }
        }
    }
}
