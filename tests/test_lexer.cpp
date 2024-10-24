#include <vector>
#include <map>
#include "token.hpp"
#include "lexer.hpp"
#include "catch_amalgamated.hpp"

TEST_CASE("Lexer works correctly", "[lexer]")
{
    SECTION("Integers are tokenized properly")
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

    SECTION("Floating point numbers are tokenized properly")
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

    SECTION("Strings are tokenized properly")
    {
        std::string sourceCode = "Hello, world !";
        Lexer lexer = Lexer('"' + sourceCode + '"');
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token stringToken = tokens.at(0);

        REQUIRE(stringToken.type == TokenType::STRING);
        REQUIRE(stringToken.value == sourceCode);
        REQUIRE(stringToken.metadata.length == sourceCode.size() + 2);
    }

    SECTION("Booleans are tokenized properly")
    {
        std::vector<std::string> booleans = {"true", "false"};

        for (const std::string boolean : booleans)
        {
            Lexer lexer = Lexer(boolean);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token booleanToken = tokens.at(0);

            REQUIRE(booleanToken.type == TokenType::BOOLEAN);
            REQUIRE(booleanToken.value == boolean);
            REQUIRE(booleanToken.metadata.length == boolean.size());
        }
    }

    SECTION("Identifiers are tokenized properly")
    {
        std::string sourceCode = "my_variable";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token identifierToken = tokens.at(0);

        REQUIRE(identifierToken.type == TokenType::IDENTIFIER);
        REQUIRE(identifierToken.value == sourceCode);
        REQUIRE(identifierToken.metadata.length == sourceCode.size());
    }
}
