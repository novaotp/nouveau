#include <vector>
#include <map>
#include "token.hpp"
#include "lexer.hpp"
#include "catch_amalgamated.hpp"

TEST_CASE("Lexer works correctly", "[lexer]")
{
    SECTION("Integer is tokenized properly")
    {
        std::string sourceCode = "69";
        Lexer lexer = Lexer(sourceCode);

        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token intToken = tokens.at(0);

        REQUIRE(intToken.type == TokenType::INTEGER);
        REQUIRE(intToken.value == "69");
        REQUIRE(intToken.metadata.length == 2);
    }

    SECTION("Floating point number is tokenized properly")
    {
        std::string sourceCode = "3.14";
        Lexer lexer = Lexer(sourceCode);

        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);
        Token floatToken = tokens.at(0);

        REQUIRE(floatToken.type == TokenType::FLOAT);
        REQUIRE(floatToken.value == "3.14");
        REQUIRE(floatToken.metadata.length == 4);
    }

    SECTION("Operators are tokenized properly")
    {
        std::map<std::string, TokenType> operatorCases = {
            {"+", TokenType::ADDITION_OPERATOR},
            {"-", TokenType::SUBTRACTION_OPERATOR},
            {"*", TokenType::MULTIPLICATION_OPERATOR},
            {"/", TokenType::DIVISION_OPERATOR}};

        for (const auto &[sourceCode, expectedType] : operatorCases)
        {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);
            Token operatorToken = tokens.at(0);

            REQUIRE(operatorToken.type == expectedType);
            REQUIRE(operatorToken.value == sourceCode);
            REQUIRE(operatorToken.metadata.length == 1);
        }
    }

    SECTION("Whitespace is handled properly")
    {
        std::vector<std::string> whitespaceCases = {
            " ",
            "\n",
            "\t",
            "\r",
            "\v",
            "\f",
            "  \t\n"};

        for (const std::string &sourceCode : whitespaceCases)
        {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 1);
            REQUIRE(tokens.at(0).type == TokenType::END_OF_FILE);
        }
    }
}
