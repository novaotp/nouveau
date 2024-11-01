#include <vector>
#include <map>
#include "token.hpp"
#include "lexer.hpp"
#include "catch_amalgamated.hpp"

TEST_CASE("Lexer works correctly", "[lexer]") {
    SECTION("Integers are tokenized properly") {
        std::string sourceCode = "69";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token intToken = tokens.at(0);

        REQUIRE(intToken.type == TokenType::INTEGER);
        REQUIRE(intToken.value == "69");
        REQUIRE(intToken.metadata.length == 2);
    }

    SECTION("Floating point numbers are tokenized properly") {
        std::string sourceCode = "3.14";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token floatToken = tokens.at(0);

        REQUIRE(floatToken.type == TokenType::FLOAT);
        REQUIRE(floatToken.value == "3.14");
        REQUIRE(floatToken.metadata.length == 4);
    }

    SECTION("Operators are tokenized properly") {
        std::map<std::string, TokenType> operatorCases = {
            { "+", TokenType::ADDITION_OPERATOR },
            { "-", TokenType::SUBTRACTION_OPERATOR },
            { "*", TokenType::MULTIPLICATION_OPERATOR },
            { "/", TokenType::DIVISION_OPERATOR },
            { "%", TokenType::MODULO_OPERATOR } };

        for (const auto& [sourceCode, expectedType] : operatorCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token operatorToken = tokens.at(0);

            REQUIRE(operatorToken.type == expectedType);
            REQUIRE(operatorToken.value == sourceCode);
            REQUIRE(operatorToken.metadata.length == 1);
        }
    }

    SECTION("Whitespace is handled properly") {
        std::vector<std::string> whitespaceCases = {
            " ",
            "\n",
            "\t",
            "\r",
            "\v",
            "\f",
            "  \t\n" };

        for (const std::string& sourceCode : whitespaceCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 1);
            REQUIRE(tokens.at(0).type == TokenType::END_OF_FILE);
        }
    }

    SECTION("Strings are tokenized properly") {
        std::string sourceCode = "Hello, world !";
        Lexer lexer = Lexer('"' + sourceCode + '"');
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token stringToken = tokens.at(0);

        REQUIRE(stringToken.type == TokenType::STRING);
        REQUIRE(stringToken.value == sourceCode);
        REQUIRE(stringToken.metadata.length == sourceCode.size() + 2);
    }

    SECTION("Booleans are tokenized properly") {
        std::vector<std::string> booleans = { "true", "false" };

        for (const std::string boolean : booleans) {
            Lexer lexer = Lexer(boolean);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token booleanToken = tokens.at(0);

            REQUIRE(booleanToken.type == TokenType::BOOLEAN);
            REQUIRE(booleanToken.value == boolean);
            REQUIRE(booleanToken.metadata.length == boolean.size());
        }
    }

    SECTION("Identifiers are tokenized properly") {
        std::string sourceCode = "my_variable";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token identifierToken = tokens.at(0);

        REQUIRE(identifierToken.type == TokenType::IDENTIFIER);
        REQUIRE(identifierToken.value == sourceCode);
        REQUIRE(identifierToken.metadata.length == sourceCode.size());
    }

    SECTION("Const and Mut are tokenized properly") {
        std::map<std::string, TokenType> keywordCases = {
            { "const", TokenType::CONST_KEYWORD },
            { "mut", TokenType::MUTABLE_KEYWORD } };

        for (const auto& [sourceCode, expectedType] : keywordCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token keywordToken = tokens.at(0);
            REQUIRE(keywordToken.type == expectedType);
            REQUIRE(keywordToken.value == sourceCode);
            REQUIRE(keywordToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Data types (string, int, float, bool) are tokenized properly") {
        std::vector<std::string> dataTypes = { "string", "int", "float", "bool" };

        for (const std::string dataType : dataTypes) {
            Lexer lexer = Lexer(dataType);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token typeToken = tokens.at(0);
            REQUIRE(typeToken.type == TokenType::TYPE);
            REQUIRE(typeToken.value == dataType);
            REQUIRE(typeToken.metadata.length == dataType.size());
        }
    }

    SECTION("Comparison operators are tokenized properly") {
        std::map<std::string, TokenType> comparisonCases = {
            { "==", TokenType::EQUAL_OPERATOR },
            { "!=", TokenType::NOT_EQUAL_OPERATOR },
            { ">=", TokenType::GREATER_OR_EQUAL_OPERATOR },
            { ">", TokenType::GREATER_THAN_OPERATOR },
            { "<=", TokenType::LESS_OR_EQUAL_OPERATOR },
            { "<", TokenType::LESS_THAN_OPERATOR } };

        for (const auto& [sourceCode, expectedType] : comparisonCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token comparisonToken = tokens.at(0);

            REQUIRE(comparisonToken.type == expectedType);
            REQUIRE(comparisonToken.value == sourceCode);
            REQUIRE(comparisonToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Logical operators are tokenized properly") {
        std::map<std::string, TokenType> logicalCases = {
            { "!", TokenType::NOT_OPERATOR },
            { "||", TokenType::OR_OPERATOR },
            { "&&", TokenType::AND_OPERATOR } };

        for (const auto& [sourceCode, expectedType] : logicalCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token logicalToken = tokens.at(0);

            REQUIRE(logicalToken.type == expectedType);
            REQUIRE(logicalToken.value == sourceCode);
            REQUIRE(logicalToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Assignment operator is tokenized properly") {
        std::string sourceCode = "=";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token assignmentToken = tokens.at(0);

        REQUIRE(assignmentToken.type == TokenType::ASSIGNMENT_OPERATOR);
        REQUIRE(assignmentToken.value == "=");
        REQUIRE(assignmentToken.metadata.length == 1);
    }

    SECTION("Conditionals are tokenized properly") {
        std::map<std::string, TokenType> conditionalCases = {
            { "if", TokenType::IF_KEYWORD },
            { "else", TokenType::ELSE_KEYWORD },
            { "else if", TokenType::ELSE_IF_KEYWORD } };

        for (const auto& [sourceCode, expectedType] : conditionalCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token conditionalToken = tokens.at(0);

            REQUIRE(conditionalToken.type == expectedType);
            REQUIRE(conditionalToken.value == sourceCode);
            REQUIRE(conditionalToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Loop keywords are tokenized properly") {
        std::map<std::string, TokenType> loopCases = {
            { "while", TokenType::WHILE_KEYWORD },
            { "for", TokenType::FOR_KEYWORD } };

        for (const auto& [sourceCode, expectedType] : loopCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token loopToken = tokens.at(0);

            REQUIRE(loopToken.type == expectedType);
            REQUIRE(loopToken.value == sourceCode);
            REQUIRE(loopToken.metadata.length == sourceCode.size());
        }
    }

    SECTION("Parentheses are tokenized properly") {
        std::map<std::string, TokenType> parenthesesCases = {
            { "(", TokenType::LEFT_PARENTHESIS },
            { ")", TokenType::RIGHT_PARENTHESIS },
            { "[", TokenType::LEFT_BRACKET },
            { "]", TokenType::RIGHT_BRACKET },
            { "{", TokenType::LEFT_BRACE },
            { "}", TokenType::RIGHT_BRACE } };

        for (const auto& [sourceCode, expectedType] : parenthesesCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token parenToken = tokens.at(0);

            REQUIRE(parenToken.type == expectedType);
            REQUIRE(parenToken.value == sourceCode);
            REQUIRE(parenToken.metadata.length == 1);
        }
    }

    SECTION("Punctuations are tokenized properly") {
        std::map<std::string, TokenType> punctuationCases = {
            { ":", TokenType::COLON },
            { ";", TokenType::SEMI_COLON },
            { ".", TokenType::DOT },
            { ",", TokenType::COMMA } };

        for (const auto& [sourceCode, expectedType] : punctuationCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);
            Token punctuationToken = tokens.at(0);

            REQUIRE(punctuationToken.type == expectedType);
            REQUIRE(punctuationToken.value == sourceCode);
            REQUIRE(punctuationToken.metadata.length == 1);
        }
    }

    SECTION("Compound assignment operators are tokenized properly") {
        std::map<std::string, TokenType> compoundAssignmentCases = {
            { "+=", TokenType::ADDITION_ASSIGNMENT_OPERATOR },
            { "-=", TokenType::SUBTRACTION_ASSIGNMENT_OPERATOR },
            { "*=", TokenType::MULTIPLICATION_ASSIGNMENT_OPERATOR },
            { "/=", TokenType::DIVISION_ASSIGNMENT_OPERATOR },
            { "%=", TokenType::MODULO_ASSIGNMENT_OPERATOR } };

        for (const auto& [sourceCode, expectedType] : compoundAssignmentCases) {
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            REQUIRE(tokens.size() == 2);

            Token compoundAssignmentToken = tokens.at(0);

            REQUIRE(compoundAssignmentToken.type == expectedType);
            REQUIRE(compoundAssignmentToken.value == sourceCode);
            REQUIRE(compoundAssignmentToken.metadata.length == 2);
        }
    }

    SECTION("Null keyword are tokenized properly") {
        std::string sourceCode = "null";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token nullToken = tokens.at(0);

        REQUIRE(nullToken.type == TokenType::NULL_KEYWORD);
        REQUIRE(nullToken.value == sourceCode);
        REQUIRE(nullToken.metadata.length == sourceCode.size());
    }

    SECTION("Function keyword are tokenized properly") {
        std::string sourceCode = "fn";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token fnToken = tokens.at(0);

        REQUIRE(fnToken.type == TokenType::FUNCTION_KEYWORD);
        REQUIRE(fnToken.value == sourceCode);
        REQUIRE(fnToken.metadata.length == sourceCode.size());
    }

    SECTION("Triple-slash comments are ignored properly") {
        std::string sourceCode = "/// This is a comment\nmut int x = 10;";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 7);

        Token mutToken = tokens.at(0);
        REQUIRE(mutToken.type == TokenType::MUTABLE_KEYWORD);
        REQUIRE(mutToken.value == "mut");

        Token intToken = tokens.at(1);
        REQUIRE(intToken.type == TokenType::TYPE);
        REQUIRE(intToken.value == "int");

        Token identifierToken = tokens.at(2);
        REQUIRE(identifierToken.type == TokenType::IDENTIFIER);
        REQUIRE(identifierToken.value == "x");

        Token equalToken = tokens.at(3);
        REQUIRE(equalToken.type == TokenType::ASSIGNMENT_OPERATOR);
        REQUIRE(equalToken.value == "=");

        Token numberToken = tokens.at(4);
        REQUIRE(numberToken.type == TokenType::INTEGER);
        REQUIRE(numberToken.value == "10");

        Token semicolonToken = tokens.at(5);
        REQUIRE(semicolonToken.type == TokenType::SEMI_COLON);
        REQUIRE(semicolonToken.value == ";");
    }

    SECTION("Break keyword is tokenized properly") {
        std::string sourceCode = "break";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token breakToken = tokens.at(0);

        REQUIRE(breakToken.type == TokenType::BREAK_KEYWORD);
        REQUIRE(breakToken.value == "break");
        REQUIRE(breakToken.metadata.length == 5);
    }

    SECTION("Continue keyword is tokenized properly") {
        std::string sourceCode = "continue";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token continueToken = tokens.at(0);

        REQUIRE(continueToken.type == TokenType::CONTINUE_KEYWORD);
        REQUIRE(continueToken.value == "continue");
        REQUIRE(continueToken.metadata.length == 8);
    }

    SECTION("Return keyword is tokenized properly") {
        std::string sourceCode = "return";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        REQUIRE(tokens.size() == 2);

        Token returnToken = tokens.at(0);

        REQUIRE(returnToken.type == TokenType::RETURN_KEYWORD);
        REQUIRE(returnToken.value == "return");
        REQUIRE(returnToken.metadata.length == 6);
    }
}
