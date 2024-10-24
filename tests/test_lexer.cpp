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

    SECTION("Const and Mut are tokenized properly")
    {
        std::map<std::string, TokenType> keywordCases = {
            {"const", TokenType::CONST_KEYWORD},
            {"mut", TokenType::MUTABLE_KEYWORD}};

        for (const auto &[sourceCode, expectedType] : keywordCases)
        {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token keywordToken = tokens.at(0);
            REQUIRE(keywordToken.type == expectedType);
            REQUIRE(keywordToken.value == sourceCode);
            REQUIRE(keywordToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Data types (string, int, float, bool) are tokenized properly")
    {
        std::vector<std::string> dataTypes = {"string", "int", "float", "bool"};

        for (const std::string dataType : dataTypes)
        {
            Lexer lexer = Lexer(dataType);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token typeToken = tokens.at(0);
            REQUIRE(typeToken.type == TokenType::TYPE);
            REQUIRE(typeToken.value == dataType);
            REQUIRE(typeToken.metadata.length == dataType.size());
        }
    }

    SECTION("Comparison operators are tokenized properly")
    {
        std::map<std::string, TokenType> comparisonCases = {
            {"==", TokenType::EQUAL_OPERATOR},
            {"!=", TokenType::NOT_EQUAL_OPERATOR},
            {">=", TokenType::GREATER_OR_EQUAL_OPERATOR},
            {">", TokenType::GREATER_THAN_OPERATOR},
            {"<=", TokenType::LESS_OR_EQUAL_OPERATOR},
            {"<", TokenType::LESS_THAN_OPERATOR}};

        for (const auto &[sourceCode, expectedType] : comparisonCases)
        {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token comparisonToken = tokens.at(0);

            REQUIRE(comparisonToken.type == expectedType);
            REQUIRE(comparisonToken.value == sourceCode);
            REQUIRE(comparisonToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Logical operators are tokenized properly")
    {
        std::map<std::string, TokenType> logicalCases = {
            {"!", TokenType::NOT_OPERATOR},
            {"||", TokenType::OR_OPERATOR},
            {"&&", TokenType::AND_OPERATOR}};

        for (const auto &[sourceCode, expectedType] : logicalCases)
        {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token logicalToken = tokens.at(0);

            REQUIRE(logicalToken.type == expectedType);
            REQUIRE(logicalToken.value == sourceCode);
            REQUIRE(logicalToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Assignment operator is tokenized properly")
    {
        std::string sourceCode = "=";
        Lexer lexer = Lexer(sourceCode);

        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token assignmentToken = tokens.at(0);

        REQUIRE(assignmentToken.type == TokenType::ASSIGNMENT_OPERATOR);
        REQUIRE(assignmentToken.value == "=");
        REQUIRE(assignmentToken.metadata.length == 1);
    }

    SECTION("Conditionals are tokenized properly")
    {
        std::map<std::string, TokenType> conditionalCases = {
            {"if", TokenType::IF_KEYWORD},
            {"else", TokenType::ELSE_KEYWORD},
            {"else if", TokenType::ELSE_IF_KEYWORD}};

        for (const auto &[sourceCode, expectedType] : conditionalCases)
        {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token conditionalToken = tokens.at(0);

            REQUIRE(conditionalToken.type == expectedType);
            REQUIRE(conditionalToken.value == sourceCode);
            REQUIRE(conditionalToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Loop keywords are tokenized properly")
    {
        std::map<std::string, TokenType> loopCases = {
            {"while", TokenType::WHILE_KEYWORD},
            {"for", TokenType::FOR_KEYWORD}};

        for (const auto &[sourceCode, expectedType] : loopCases)
        {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token loopToken = tokens.at(0);

            REQUIRE(loopToken.type == expectedType);
            REQUIRE(loopToken.value == sourceCode);
            REQUIRE(loopToken.metadata.length == sourceCode.size());
        }
    }
}
