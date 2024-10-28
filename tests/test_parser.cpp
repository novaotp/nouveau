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

    SECTION("Complex arithmetic operations are parsed properly") {
        std::string sourceCode = "420 + 69 * 3.14 - 7";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<ArithmeticOperation>(*expressionPtr));

        // Top-level ((420 + (69 * 3.14)) - 7)
        ArithmeticOperation topLevelOperation = std::move(std::get<ArithmeticOperation>(*expressionPtr));
        REQUIRE(topLevelOperation.op == "-");

        // Left side should (420 + (69 * 3.14))
        REQUIRE(std::holds_alternative<ArithmeticOperation>(*topLevelOperation.lhs));
        ArithmeticOperation additiveOperation = std::move(std::get<ArithmeticOperation>(*topLevelOperation.lhs));
        REQUIRE(additiveOperation.op == "+");

        // Left side of the "+" (420)
        REQUIRE(std::holds_alternative<Literal>(*additiveOperation.lhs));
        Literal leftLiteralWrapper = std::get<Literal>(*additiveOperation.lhs);
        REQUIRE(std::holds_alternative<IntLiteral>(leftLiteralWrapper));
        IntLiteral leftLiteral = std::get<IntLiteral>(leftLiteralWrapper);
        REQUIRE(leftLiteral.value == 420);

        // Right side of the "+" (69 * 3.14)
        REQUIRE(std::holds_alternative<ArithmeticOperation>(*additiveOperation.rhs));
        ArithmeticOperation multiplicativeOperation = std::move(std::get<ArithmeticOperation>(*additiveOperation.rhs));
        REQUIRE(multiplicativeOperation.op == "*");

        // Left side of the "*" (69)
        REQUIRE(std::holds_alternative<Literal>(*multiplicativeOperation.lhs));
        Literal multLeftLiteralWrapper = std::get<Literal>(*multiplicativeOperation.lhs);
        REQUIRE(std::holds_alternative<IntLiteral>(multLeftLiteralWrapper));
        IntLiteral multLeftLiteral = std::get<IntLiteral>(multLeftLiteralWrapper);
        REQUIRE(multLeftLiteral.value == 69);

        // Right side of the "*" (3.14)
        REQUIRE(std::holds_alternative<Literal>(*multiplicativeOperation.rhs));
        Literal multRightLiteralWrapper = std::get<Literal>(*multiplicativeOperation.rhs);
        REQUIRE(std::holds_alternative<FloatLiteral>(multRightLiteralWrapper));
        FloatLiteral multRightLiteral = std::get<FloatLiteral>(multRightLiteralWrapper);
        REQUIRE(multRightLiteral.value == 3.14f);

        // Right side of the "-" (7)
        REQUIRE(std::holds_alternative<Literal>(*topLevelOperation.rhs));
        Literal rightLiteralWrapper = std::get<Literal>(*topLevelOperation.rhs);
        REQUIRE(std::holds_alternative<IntLiteral>(rightLiteralWrapper));
        IntLiteral rightLiteral = std::get<IntLiteral>(rightLiteralWrapper);
        REQUIRE(rightLiteral.value == 7);
    }

    SECTION("Complex arithmetic operations with parentheses are parsed properly") {
        std::string sourceCode = "(420 + 69) * (3.14 - 7)";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<ArithmeticOperation>(*expressionPtr));

        // Top-level ((420 + 69) * (3.14 - 7))
        ArithmeticOperation topLevelOperation = std::move(std::get<ArithmeticOperation>(*expressionPtr));
        REQUIRE(topLevelOperation.op == "*");

        // Left side should be (420 + 69)
        REQUIRE(std::holds_alternative<ArithmeticOperation>(*topLevelOperation.lhs));
        ArithmeticOperation additiveOperation = std::move(std::get<ArithmeticOperation>(*topLevelOperation.lhs));
        REQUIRE(additiveOperation.op == "+");

        // Left side of the "+" (420)
        REQUIRE(std::holds_alternative<Literal>(*additiveOperation.lhs));
        Literal leftLiteralWrapper = std::get<Literal>(*additiveOperation.lhs);
        REQUIRE(std::holds_alternative<IntLiteral>(leftLiteralWrapper));
        IntLiteral leftLiteral = std::get<IntLiteral>(leftLiteralWrapper);
        REQUIRE(leftLiteral.value == 420);

        // Right side of the "+" (69)
        REQUIRE(std::holds_alternative<Literal>(*additiveOperation.rhs));
        Literal rightLiteralWrapper = std::get<Literal>(*additiveOperation.rhs);
        REQUIRE(std::holds_alternative<IntLiteral>(rightLiteralWrapper));
        IntLiteral rightLiteral = std::get<IntLiteral>(rightLiteralWrapper);
        REQUIRE(rightLiteral.value == 69);

        // Right side of the "*" (3.14 - 7)
        REQUIRE(std::holds_alternative<ArithmeticOperation>(*topLevelOperation.rhs));
        ArithmeticOperation subtractiveOperation = std::move(std::get<ArithmeticOperation>(*topLevelOperation.rhs));
        REQUIRE(subtractiveOperation.op == "-");

        // Left side of the "-" (3.14)
        REQUIRE(std::holds_alternative<Literal>(*subtractiveOperation.lhs));
        Literal subLeftLiteralWrapper = std::get<Literal>(*subtractiveOperation.lhs);
        REQUIRE(std::holds_alternative<FloatLiteral>(subLeftLiteralWrapper));
        FloatLiteral subLeftLiteral = std::get<FloatLiteral>(subLeftLiteralWrapper);
        REQUIRE(subLeftLiteral.value == 3.14f);

        // Right side of the "-" (7)
        REQUIRE(std::holds_alternative<Literal>(*subtractiveOperation.rhs));
        Literal subRightLiteralWrapper = std::get<Literal>(*subtractiveOperation.rhs);
        REQUIRE(std::holds_alternative<IntLiteral>(subRightLiteralWrapper));
        IntLiteral subRightLiteral = std::get<IntLiteral>(subRightLiteralWrapper);
        REQUIRE(subRightLiteral.value == 7);
    }

    SECTION("Comparison operations are parsed properly") {
        std::string sourceCode = "69 > 3.14";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<ComparisonOperation>(*expressionPtr));
        ComparisonOperation comparisonOperation = std::move(std::get<ComparisonOperation>(*expressionPtr));

        auto& lhs = comparisonOperation.lhs;
        REQUIRE(std::holds_alternative<Literal>(*lhs));

        Literal lhsLiteral = std::get<Literal>(*lhs);
        REQUIRE(std::holds_alternative<IntLiteral>(lhsLiteral));

        IntLiteral lhsIntLiteral = std::get<IntLiteral>(lhsLiteral);
        REQUIRE(lhsIntLiteral.value == 69);

        REQUIRE(comparisonOperation.op == ">");

        auto& rhs = comparisonOperation.rhs;
        REQUIRE(std::holds_alternative<Literal>(*rhs));

        Literal rhsLiteral = std::get<Literal>(*rhs);
        REQUIRE(std::holds_alternative<FloatLiteral>(rhsLiteral));

        FloatLiteral rhsFloatLiteral = std::get<FloatLiteral>(rhsLiteral);
        REQUIRE(rhsFloatLiteral.value == 3.14f);
    }

    SECTION("Logical NOT operations are parsed properly") {
        std::string sourceCode = "!false";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<LogicalNotOperation>(*expressionPtr));

        LogicalNotOperation notOperation = std::move(std::get<LogicalNotOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<Literal>(*notOperation.expression));

        Literal literal = std::get<Literal>(*notOperation.expression);
        REQUIRE(std::holds_alternative<BooleanLiteral>(literal));

        BooleanLiteral boolLiteral = std::get<BooleanLiteral>(literal);
        REQUIRE(boolLiteral.value == false);
    }

    SECTION("Logical AND operations are parsed properly") {
        std::string sourceCode = "true && false";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        BinaryOperation andOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<Literal>(*andOperation.lhs));

        Literal lhsLiteral = std::get<Literal>(*andOperation.lhs);
        REQUIRE(std::holds_alternative<BooleanLiteral>(lhsLiteral));

        BooleanLiteral lhsBooleanLiteral = std::get<BooleanLiteral>(lhsLiteral);
        REQUIRE(lhsBooleanLiteral.value == true);

        REQUIRE(andOperation.op == "&&");

        auto& rhs = andOperation.rhs;
        REQUIRE(std::holds_alternative<Literal>(*rhs));

        Literal rhsLiteral = std::get<Literal>(*rhs);
        REQUIRE(std::holds_alternative<BooleanLiteral>(rhsLiteral));

        BooleanLiteral rhsBooleanLiteral = std::get<BooleanLiteral>(rhsLiteral);
        REQUIRE(rhsBooleanLiteral.value == false);
    }

    SECTION("Logical OR operations are parsed properly") {
        std::string sourceCode = "false || true";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        BinaryOperation andOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<Literal>(*andOperation.lhs));

        Literal lhsLiteral = std::get<Literal>(*andOperation.lhs);
        REQUIRE(std::holds_alternative<BooleanLiteral>(lhsLiteral));

        BooleanLiteral lhsBooleanLiteral = std::get<BooleanLiteral>(lhsLiteral);
        REQUIRE(lhsBooleanLiteral.value == false);

        REQUIRE(andOperation.op == "||");

        auto& rhs = andOperation.rhs;
        REQUIRE(std::holds_alternative<Literal>(*rhs));

        Literal rhsLiteral = std::get<Literal>(*rhs);
        REQUIRE(std::holds_alternative<BooleanLiteral>(rhsLiteral));

        BooleanLiteral rhsBooleanLiteral = std::get<BooleanLiteral>(rhsLiteral);
        REQUIRE(rhsBooleanLiteral.value == true);
    }

    SECTION("Variable declarations are handled properly") {
        std::string sourceCode = "const string message = \"Hello, World !\";";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body.at(0);
        REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(firstElement));

        auto& statement = std::get<std::unique_ptr<Statement>>(firstElement);
        REQUIRE(std::holds_alternative<VariableDeclaration>(*statement));

        VariableDeclaration variableAssignment = std::move(std::get<VariableDeclaration>(*statement));

        REQUIRE(variableAssignment.isMutable == false);
        REQUIRE(variableAssignment.type == "string");
        REQUIRE(variableAssignment.identifier == "message");
        REQUIRE(variableAssignment.value.has_value() == true);

        auto& valueExpressionPtr = variableAssignment.value.value();
        REQUIRE(std::holds_alternative<Literal>(*valueExpressionPtr));

        Literal valueLiteral = std::get<Literal>(*valueExpressionPtr);
        REQUIRE(std::holds_alternative<StringLiteral>(valueLiteral));

        StringLiteral stringLiteral = std::get<StringLiteral>(valueLiteral);
        REQUIRE(stringLiteral.value == "Hello, World !");
    }

    SECTION("Variable assignments are handled properly") {
        std::string sourceCode = "message = \"Hello, World !\";";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body.at(0);
        REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(firstElement));

        auto& statement = std::get<std::unique_ptr<Statement>>(firstElement);
        REQUIRE(std::holds_alternative<VariableAssignment>(*statement));

        VariableAssignment variableAssignment = std::move(std::get<VariableAssignment>(*statement));

        REQUIRE(variableAssignment.identifier == "message");
        REQUIRE(variableAssignment.value.has_value() == true);

        auto& valueExpressionPtr = variableAssignment.value.value();
        REQUIRE(std::holds_alternative<Literal>(*valueExpressionPtr));

        Literal valueLiteral = std::get<Literal>(*valueExpressionPtr);
        REQUIRE(std::holds_alternative<StringLiteral>(valueLiteral));

        StringLiteral stringLiteral = std::get<StringLiteral>(valueLiteral);
        REQUIRE(stringLiteral.value == "Hello, World !");
    }
}
