#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <vector>
#include <map>
#include "token.hpp"
#include "lexer.hpp"
#include "parser.hpp"

TEST_CASE("Parser works correctly", "[parser]") {
    SECTION("String literals are parsed properly") {
        std::string sourceCode = "\"Hello, World!\"";
        Lexer lexer = Lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<StringLiteral>(*expressionPtr));

        StringLiteral stringLiteral = std::get<StringLiteral>(*expressionPtr);
        REQUIRE(stringLiteral.value == "Hello, World!");

        REQUIRE(stringLiteral.metadata.start.column == 1);
        REQUIRE(stringLiteral.metadata.start.line == 1);
        REQUIRE(stringLiteral.metadata.end.column == 16);
        REQUIRE(stringLiteral.metadata.end.line == 1);
    }

    SECTION("Int literals are parsed properly") {
        std::string sourceCode = "69";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<IntLiteral>(*expressionPtr));

        IntLiteral intLiteral = std::get<IntLiteral>(*expressionPtr);
        REQUIRE(intLiteral.value == 69);

        REQUIRE(intLiteral.metadata.start.column == 1);
        REQUIRE(intLiteral.metadata.start.line == 1);
        REQUIRE(intLiteral.metadata.end.column == 3);
        REQUIRE(intLiteral.metadata.end.line == 1);
    }

    SECTION("Float literals are parsed properly") {
        std::string sourceCode = "3.14";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<FloatLiteral>(*expressionPtr));

        FloatLiteral floatLiteral = std::get<FloatLiteral>(*expressionPtr);
        REQUIRE(floatLiteral.value == 3.14f);

        REQUIRE(floatLiteral.metadata.start.column == 1);
        REQUIRE(floatLiteral.metadata.start.line == 1);
        REQUIRE(floatLiteral.metadata.end.column == 5);
        REQUIRE(floatLiteral.metadata.end.line == 1);
    }

    SECTION("Boolean literals are parsed properly") {
        std::string sourceCode = "true";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BooleanLiteral>(*expressionPtr));

        BooleanLiteral booleanLiteral = std::get<BooleanLiteral>(*expressionPtr);
        REQUIRE(booleanLiteral.value == true);

        REQUIRE(booleanLiteral.metadata.start.column == 1);
        REQUIRE(booleanLiteral.metadata.start.line == 1);
        REQUIRE(booleanLiteral.metadata.end.column == 5);
        REQUIRE(booleanLiteral.metadata.end.line == 1);
    }

    SECTION("Additive arithmetic operations are parsed properly") {
        std::string sourceCode = "69 + 3.14";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));
        BinaryOperation arithmeticOperation = std::move(std::get<BinaryOperation>(*expressionPtr));

        REQUIRE(arithmeticOperation.metadata.start.column == 1);
        REQUIRE(arithmeticOperation.metadata.start.line == 1);
        REQUIRE(arithmeticOperation.metadata.end.column == 10);
        REQUIRE(arithmeticOperation.metadata.end.line == 1);

        auto& lhs = arithmeticOperation.lhs;
        REQUIRE(std::holds_alternative<IntLiteral>(*lhs));

        IntLiteral lhsIntLiteral = std::get<IntLiteral>(*lhs);
        REQUIRE(lhsIntLiteral.value == 69);

        REQUIRE(arithmeticOperation.op == BinaryOperator::ADDITION);

        auto& rhs = arithmeticOperation.rhs;
        REQUIRE(std::holds_alternative<FloatLiteral>(*rhs));

        FloatLiteral rhsIntLiteral = std::get<FloatLiteral>(*rhs);
        REQUIRE(rhsIntLiteral.value == 3.14f);
    }

    SECTION("Complex arithmetic operations are parsed properly") {
        std::string sourceCode = "420 + 69 * 3.14 - 7";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        // Top-level ((420 + (69 * 3.14)) - 7)
        BinaryOperation topLevelOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(topLevelOperation.op == BinaryOperator::SUBTRACTION);

        REQUIRE(topLevelOperation.metadata.start.column == 1);
        REQUIRE(topLevelOperation.metadata.start.line == 1);
        REQUIRE(topLevelOperation.metadata.end.column == 20);
        REQUIRE(topLevelOperation.metadata.end.line == 1);

        // Left side should (420 + (69 * 3.14))
        REQUIRE(std::holds_alternative<BinaryOperation>(*topLevelOperation.lhs));
        BinaryOperation additiveOperation = std::move(std::get<BinaryOperation>(*topLevelOperation.lhs));
        REQUIRE(additiveOperation.op == BinaryOperator::ADDITION);

        // Left side of the "+" (420)
        REQUIRE(std::holds_alternative<IntLiteral>(*additiveOperation.lhs));
        IntLiteral leftLiteral = std::get<IntLiteral>(*additiveOperation.lhs);
        REQUIRE(leftLiteral.value == 420);

        // Right side of the "+" (69 * 3.14)
        REQUIRE(std::holds_alternative<BinaryOperation>(*additiveOperation.rhs));
        BinaryOperation multiplicativeOperation = std::move(std::get<BinaryOperation>(*additiveOperation.rhs));
        REQUIRE(multiplicativeOperation.op == BinaryOperator::MULTIPLICATION);

        // Left side of the "*" (69)
        REQUIRE(std::holds_alternative<IntLiteral>(*multiplicativeOperation.lhs));
        IntLiteral multLeftLiteral = std::get<IntLiteral>(*multiplicativeOperation.lhs);
        REQUIRE(multLeftLiteral.value == 69);

        // Right side of the "*" (3.14)
        REQUIRE(std::holds_alternative<FloatLiteral>(*multiplicativeOperation.rhs));
        FloatLiteral multRightLiteral = std::get<FloatLiteral>(*multiplicativeOperation.rhs);
        REQUIRE(multRightLiteral.value == 3.14f);

        // Right side of the "-" (7)
        REQUIRE(std::holds_alternative<IntLiteral>(*topLevelOperation.rhs));
        IntLiteral rightLiteral = std::get<IntLiteral>(*topLevelOperation.rhs);
        REQUIRE(rightLiteral.value == 7);
    }

    SECTION("Complex arithmetic operations with parentheses are parsed properly") {
        std::string sourceCode = "(420 + 69) * (3.14 - 7)";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        // Top-level ((420 + 69) * (3.14 - 7))
        BinaryOperation topLevelOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(topLevelOperation.op == BinaryOperator::MULTIPLICATION);

        REQUIRE(topLevelOperation.metadata.start.column == 1);
        REQUIRE(topLevelOperation.metadata.start.line == 1);
        REQUIRE(topLevelOperation.metadata.end.column == 24);
        REQUIRE(topLevelOperation.metadata.end.line == 1);

        // Left side should be (420 + 69)
        REQUIRE(std::holds_alternative<BinaryOperation>(*topLevelOperation.lhs));
        BinaryOperation additiveOperation = std::move(std::get<BinaryOperation>(*topLevelOperation.lhs));
        REQUIRE(additiveOperation.op == BinaryOperator::ADDITION);

        // Left side of the "+" (420)
        REQUIRE(std::holds_alternative<IntLiteral>(*additiveOperation.lhs));
        IntLiteral leftLiteral = std::get<IntLiteral>(*additiveOperation.lhs);
        REQUIRE(leftLiteral.value == 420);

        // Right side of the "+" (69)
        REQUIRE(std::holds_alternative<IntLiteral>(*additiveOperation.rhs));
        IntLiteral rightLiteral = std::get<IntLiteral>(*additiveOperation.rhs);
        REQUIRE(rightLiteral.value == 69);

        // Right side of the "*" (3.14 - 7)
        REQUIRE(std::holds_alternative<BinaryOperation>(*topLevelOperation.rhs));
        BinaryOperation subtractiveOperation = std::move(std::get<BinaryOperation>(*topLevelOperation.rhs));
        REQUIRE(subtractiveOperation.op == BinaryOperator::SUBTRACTION);

        // Left side of the "-" (3.14)
        REQUIRE(std::holds_alternative<FloatLiteral>(*subtractiveOperation.lhs));
        FloatLiteral subLeftLiteral = std::get<FloatLiteral>(*subtractiveOperation.lhs);
        REQUIRE(subLeftLiteral.value == 3.14f);

        // Right side of the "-" (7)
        REQUIRE(std::holds_alternative<IntLiteral>(*subtractiveOperation.rhs));
        IntLiteral subRightLiteral = std::get<IntLiteral>(*subtractiveOperation.rhs);
        REQUIRE(subRightLiteral.value == 7);
    }

    SECTION("Comparison operations are parsed properly") {
        std::string sourceCode = "69 > 3.14";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));
        BinaryOperation comparisonOperation = std::move(std::get<BinaryOperation>(*expressionPtr));

        REQUIRE(comparisonOperation.metadata.start.column == 1);
        REQUIRE(comparisonOperation.metadata.start.line == 1);
        REQUIRE(comparisonOperation.metadata.end.column == 10);
        REQUIRE(comparisonOperation.metadata.end.line == 1);

        auto& lhs = comparisonOperation.lhs;
        REQUIRE(std::holds_alternative<IntLiteral>(*lhs));

        IntLiteral lhsIntLiteral = std::get<IntLiteral>(*lhs);
        REQUIRE(lhsIntLiteral.value == 69);

        REQUIRE(comparisonOperation.op == BinaryOperator::GREATER_THAN);

        auto& rhs = comparisonOperation.rhs;
        REQUIRE(std::holds_alternative<FloatLiteral>(*rhs));

        FloatLiteral rhsFloatLiteral = std::get<FloatLiteral>(*rhs);
        REQUIRE(rhsFloatLiteral.value == 3.14f);
    }

    SECTION("Logical NOT operations are parsed properly") {
        std::string sourceCode = "!false";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<LogicalNotOperation>(*expressionPtr));

        LogicalNotOperation notOperation = std::move(std::get<LogicalNotOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<BooleanLiteral>(*notOperation.expression));

        REQUIRE(notOperation.metadata.start.column == 1);
        REQUIRE(notOperation.metadata.start.line == 1);
        REQUIRE(notOperation.metadata.end.column == 7);
        REQUIRE(notOperation.metadata.end.line == 1);

        BooleanLiteral boolLiteral = std::get<BooleanLiteral>(*notOperation.expression);
        REQUIRE(boolLiteral.value == false);
    }

    SECTION("Logical AND operations are parsed properly") {
        std::string sourceCode = "true && false";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        BinaryOperation andOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<BooleanLiteral>(*andOperation.lhs));

        REQUIRE(andOperation.metadata.start.column == 1);
        REQUIRE(andOperation.metadata.start.line == 1);
        REQUIRE(andOperation.metadata.end.column == 14);
        REQUIRE(andOperation.metadata.end.line == 1);

        BooleanLiteral lhsBooleanLiteral = std::get<BooleanLiteral>(*andOperation.lhs);
        REQUIRE(lhsBooleanLiteral.value == true);

        REQUIRE(andOperation.op == BinaryOperator::AND);

        auto& rhs = andOperation.rhs;
        REQUIRE(std::holds_alternative<BooleanLiteral>(*rhs));

        BooleanLiteral rhsBooleanLiteral = std::get<BooleanLiteral>(*rhs);
        REQUIRE(rhsBooleanLiteral.value == false);
    }

    SECTION("Logical OR operations are parsed properly") {
        std::string sourceCode = "false || true";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        BinaryOperation orOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<BooleanLiteral>(*orOperation.lhs));

        REQUIRE(orOperation.metadata.start.column == 1);
        REQUIRE(orOperation.metadata.start.line == 1);
        REQUIRE(orOperation.metadata.end.column == 14);
        REQUIRE(orOperation.metadata.end.line == 1);

        BooleanLiteral lhsBooleanLiteral = std::get<BooleanLiteral>(*orOperation.lhs);
        REQUIRE(lhsBooleanLiteral.value == false);

        REQUIRE(orOperation.op == BinaryOperator::OR);

        auto& rhs = orOperation.rhs;
        REQUIRE(std::holds_alternative<BooleanLiteral>(*rhs));

        BooleanLiteral rhsBooleanLiteral = std::get<BooleanLiteral>(*rhs);
        REQUIRE(rhsBooleanLiteral.value == true);
    }

    SECTION("Variable declarations are handled properly") {
        std::string sourceCode = "string message = \"Hello, World !\";";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body.at(0);
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstElement));

        auto& statement = std::get<std::shared_ptr<Statement>>(firstElement);
        REQUIRE(std::holds_alternative<VariableDeclaration>(*statement));

        VariableDeclaration variableDeclaration = std::move(std::get<VariableDeclaration>(*statement));

        REQUIRE(variableDeclaration.isMutable == false);

        std::visit([](const auto& typePtr) {
            REQUIRE(typePtr->compare(std::make_shared<StringType>()) == true);
        }, variableDeclaration.type);

        REQUIRE(variableDeclaration.identifier == "message");
        REQUIRE(variableDeclaration.value.has_value() == true);

        REQUIRE(variableDeclaration.metadata.start.column == 1);
        REQUIRE(variableDeclaration.metadata.start.line == 1);
        REQUIRE(variableDeclaration.metadata.end.column == 35);
        REQUIRE(variableDeclaration.metadata.end.line == 1);

        auto& valueExpressionPtr = variableDeclaration.value.value();
        REQUIRE(std::holds_alternative<StringLiteral>(*valueExpressionPtr));

        StringLiteral stringLiteral = std::get<StringLiteral>(*valueExpressionPtr);
        REQUIRE(stringLiteral.value == "Hello, World !");
    }

    SECTION("Variable assignments are handled properly") {
        std::string sourceCode = "message = \"Hello, World !\";";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body.at(0);
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstElement));

        auto& statement = std::get<std::shared_ptr<Statement>>(firstElement);
        REQUIRE(std::holds_alternative<VariableAssignment>(*statement));

        VariableAssignment variableAssignment = std::move(std::get<VariableAssignment>(*statement));

        REQUIRE(variableAssignment.metadata.start.column == 1);
        REQUIRE(variableAssignment.metadata.start.line == 1);
        REQUIRE(variableAssignment.metadata.end.column == 28);
        REQUIRE(variableAssignment.metadata.end.line == 1);

        REQUIRE(variableAssignment.identifier == "message");

        REQUIRE(std::holds_alternative<StringLiteral>(*variableAssignment.value));
        StringLiteral stringLiteral = std::get<StringLiteral>(*variableAssignment.value);

        REQUIRE(stringLiteral.value == "Hello, World !");
    }
}
