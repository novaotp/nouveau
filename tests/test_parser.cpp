#include <iostream>
#include <vector>
#include <map>
#include "token.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "catch_amalgamated.hpp"

TEST_CASE("Parser works correctly", "[parser]") {
    SECTION("String literals are parsed properly") {
        std::string sourceCode = "\"Hello, World!\"";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser = Parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<Literal>(*expressionPtr));

        Literal literal = std::get<Literal>(*expressionPtr);
        REQUIRE(std::holds_alternative<StringLiteral>(literal));

        StringLiteral stringLiteral = std::get<StringLiteral>(literal);
        REQUIRE(stringLiteral.value == "Hello, World!");
    }

    SECTION("Int literals are parsed properly") {
        std::string sourceCode = "69";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<Literal>(*expressionPtr));

        Literal literal = std::get<Literal>(*expressionPtr);
        REQUIRE(std::holds_alternative<IntLiteral>(literal));

        IntLiteral intLiteral = std::get<IntLiteral>(literal);
        REQUIRE(intLiteral.value == 69);
    }

    SECTION("Float literals are parsed properly") {
        std::string sourceCode = "3.14";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<Literal>(*expressionPtr));

        Literal literal = std::get<Literal>(*expressionPtr);
        REQUIRE(std::holds_alternative<FloatLiteral>(literal));

        FloatLiteral floatLiteral = std::get<FloatLiteral>(literal);
        REQUIRE(floatLiteral.value == 3.14f);
    }

    SECTION("Boolean literals are parsed properly") {
        std::string sourceCode = "true";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<Literal>(*expressionPtr));

        Literal literal = std::get<Literal>(*expressionPtr);
        REQUIRE(std::holds_alternative<BooleanLiteral>(literal));

        BooleanLiteral booleanLiteral = std::get<BooleanLiteral>(literal);
        REQUIRE(booleanLiteral.value == true);
    }

    SECTION("Additive arithmetic operations are parsed properly") {
        std::string sourceCode = "69 + 3.14";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<ArithmeticOperation>(*expressionPtr));
        ArithmeticOperation arithmeticOperation = std::move(std::get<ArithmeticOperation>(*expressionPtr));

        auto& lhs = arithmeticOperation.lhs;
        REQUIRE(std::holds_alternative<Literal>(*lhs));

        Literal lhsLiteral = std::get<Literal>(*lhs);
        REQUIRE(std::holds_alternative<IntLiteral>(lhsLiteral));

        IntLiteral lhsIntLiteral = std::get<IntLiteral>(lhsLiteral);
        REQUIRE(lhsIntLiteral.value == 69);

        REQUIRE(arithmeticOperation.op == "+");

        auto& rhs = arithmeticOperation.rhs;
        REQUIRE(std::holds_alternative<Literal>(*rhs));

        Literal rhsLiteral = std::get<Literal>(*rhs);
        REQUIRE(std::holds_alternative<FloatLiteral>(rhsLiteral));

        FloatLiteral rhsIntLiteral = std::get<FloatLiteral>(rhsLiteral);
        REQUIRE(rhsIntLiteral.value == 3.14f);
    }
}
