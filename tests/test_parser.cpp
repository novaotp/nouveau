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

    SECTION("Vectors are parsed properly") {
        std::string sourceCode = "[69, false, \"hello\", 3.14]";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<Vector>(*expressionPtr));

        const Vector& vector = std::get<Vector>(*expressionPtr);

        REQUIRE(vector.metadata.start.column == 1);
        REQUIRE(vector.metadata.start.line == 1);
        REQUIRE(vector.metadata.end.column == 27);
        REQUIRE(vector.metadata.end.line == 1);

        const Expression& expr1 = *vector.values[0];
        REQUIRE(std::holds_alternative<IntLiteral>(expr1));

        IntLiteral intLiteral = std::get<IntLiteral>(expr1);
        REQUIRE(intLiteral.value == 69);

        const Expression& expr2 = *vector.values[1];
        REQUIRE(std::holds_alternative<BooleanLiteral>(expr2));

        BooleanLiteral booleanLiteral = std::get<BooleanLiteral>(expr2);
        REQUIRE(booleanLiteral.value == false);

        const Expression& expr3 = *vector.values[2];
        REQUIRE(std::holds_alternative<StringLiteral>(expr3));

        StringLiteral stringLiteral = std::get<StringLiteral>(expr3);
        REQUIRE(stringLiteral.value == "hello");

        const Expression& expr4 = *vector.values[3];
        REQUIRE(std::holds_alternative<FloatLiteral>(expr4));

        FloatLiteral floatLiteral = std::get<FloatLiteral>(expr4);
        REQUIRE(floatLiteral.value == 3.14f);
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

        REQUIRE(arithmeticOperation.op == "+");

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
        REQUIRE(topLevelOperation.op == "-");

        REQUIRE(topLevelOperation.metadata.start.column == 1);
        REQUIRE(topLevelOperation.metadata.start.line == 1);
        REQUIRE(topLevelOperation.metadata.end.column == 20);
        REQUIRE(topLevelOperation.metadata.end.line == 1);

        // Left side should (420 + (69 * 3.14))
        REQUIRE(std::holds_alternative<BinaryOperation>(*topLevelOperation.lhs));
        BinaryOperation additiveOperation = std::move(std::get<BinaryOperation>(*topLevelOperation.lhs));
        REQUIRE(additiveOperation.op == "+");

        // Left side of the "+" (420)
        REQUIRE(std::holds_alternative<IntLiteral>(*additiveOperation.lhs));
        IntLiteral leftLiteral = std::get<IntLiteral>(*additiveOperation.lhs);
        REQUIRE(leftLiteral.value == 420);

        // Right side of the "+" (69 * 3.14)
        REQUIRE(std::holds_alternative<BinaryOperation>(*additiveOperation.rhs));
        BinaryOperation multiplicativeOperation = std::move(std::get<BinaryOperation>(*additiveOperation.rhs));
        REQUIRE(multiplicativeOperation.op == "*");

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
        REQUIRE(topLevelOperation.op == "*");

        REQUIRE(topLevelOperation.metadata.start.column == 1);
        REQUIRE(topLevelOperation.metadata.start.line == 1);
        REQUIRE(topLevelOperation.metadata.end.column == 24);
        REQUIRE(topLevelOperation.metadata.end.line == 1);

        // Left side should be (420 + 69)
        REQUIRE(std::holds_alternative<BinaryOperation>(*topLevelOperation.lhs));
        BinaryOperation additiveOperation = std::move(std::get<BinaryOperation>(*topLevelOperation.lhs));
        REQUIRE(additiveOperation.op == "+");

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
        REQUIRE(subtractiveOperation.op == "-");

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

        REQUIRE(comparisonOperation.op == ">");

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

        REQUIRE(andOperation.op == "&&");

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

        REQUIRE(orOperation.op == "||");

        auto& rhs = orOperation.rhs;
        REQUIRE(std::holds_alternative<BooleanLiteral>(*rhs));

        BooleanLiteral rhsBooleanLiteral = std::get<BooleanLiteral>(*rhs);
        REQUIRE(rhsBooleanLiteral.value == true);
    }

    SECTION("Variable declarations are handled properly") {
        std::string sourceCode = "const string message = \"Hello, World !\";";
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
        REQUIRE(variableDeclaration.type == "string");
        REQUIRE(variableDeclaration.identifier == "message");
        REQUIRE(variableDeclaration.value.has_value() == true);

        REQUIRE(variableDeclaration.metadata.start.column == 1);
        REQUIRE(variableDeclaration.metadata.start.line == 1);
        REQUIRE(variableDeclaration.metadata.end.column == 41);
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
        REQUIRE(variableAssignment.value.has_value() == true);

        auto& valueExpressionPtr = variableAssignment.value.value();
        REQUIRE(std::holds_alternative<StringLiteral>(*valueExpressionPtr));

        StringLiteral stringLiteral = std::get<StringLiteral>(*valueExpressionPtr);
        REQUIRE(stringLiteral.value == "Hello, World !");
    }

    SECTION("If-else conditions are parsed properly") {
        std::string sourceCode = R"(if (true) {
    x = 10;
    "hello";
} else if (false) {
    y = 3.14;
} else {
    const int z = 20;
    null;
}
        )";

        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(program.body[0]));
        auto& mainStatement = std::get<std::shared_ptr<Statement>>(program.body[0]);

        REQUIRE(std::holds_alternative<IfStatement>(*mainStatement));
        const IfStatement& ifStatement = std::get<IfStatement>(*mainStatement);

        REQUIRE(ifStatement.metadata.start.column == 1);
        REQUIRE(ifStatement.metadata.start.line == 1);
        REQUIRE(ifStatement.metadata.end.column == 2);
        REQUIRE(ifStatement.metadata.end.line == 9);

        SECTION("Validate the main if condition") {
            REQUIRE(std::holds_alternative<BooleanLiteral>(*ifStatement.condition));
            REQUIRE(std::get<BooleanLiteral>(*ifStatement.condition).value == true);
        }

        SECTION("Validate the thenBlock") {
            REQUIRE(ifStatement.thenBlock.size() == 2);

            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(ifStatement.thenBlock[0]));
            auto& thenStmt1 = std::get<std::shared_ptr<Statement>>(ifStatement.thenBlock[0]);

            REQUIRE(std::holds_alternative<VariableAssignment>(*thenStmt1));
            REQUIRE(std::get<VariableAssignment>(*thenStmt1).identifier == "x");

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(ifStatement.thenBlock[1]));
            auto& thenExpr2 = std::get<std::shared_ptr<Expression>>(ifStatement.thenBlock[1]);

            REQUIRE(std::holds_alternative<StringLiteral>(*thenExpr2));
            REQUIRE(std::get<StringLiteral>(*thenExpr2).value == "hello");
        }

        SECTION("Validate the elseifClauses") {
            REQUIRE(ifStatement.elseifClauses.size() == 1);

            REQUIRE(std::holds_alternative<BooleanLiteral>(*ifStatement.elseifClauses[0].first));
            REQUIRE(std::get<BooleanLiteral>(*ifStatement.elseifClauses[0].first).value == false);

            REQUIRE(ifStatement.elseifClauses[0].second.size() == 1);

            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(ifStatement.elseifClauses[0].second[0]));
            auto& elseifStmt = std::get<std::shared_ptr<Statement>>(ifStatement.elseifClauses[0].second[0]);

            REQUIRE(std::holds_alternative<VariableAssignment>(*elseifStmt));
            REQUIRE(std::get<VariableAssignment>(*elseifStmt).identifier == "y");
        }

        SECTION("Validate the elseBlock") {
            REQUIRE(ifStatement.elseBlock.size() == 2);

            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(ifStatement.elseBlock[0]));
            auto& elseStmt1 = std::get<std::shared_ptr<Statement>>(ifStatement.elseBlock[0]);

            REQUIRE(std::holds_alternative<VariableDeclaration>(*elseStmt1));
            REQUIRE(std::get<VariableDeclaration>(*elseStmt1).identifier == "z");

            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(ifStatement.elseBlock[1]));
            auto& elseExpr2 = std::get<std::shared_ptr<Expression>>(ifStatement.elseBlock[1]);

            REQUIRE(std::holds_alternative<NullLiteral>(*elseExpr2));
        }
    }

    SECTION("'while' loops are parsed properly") {
        std::string sourceCode = R"(while (true) {
    x = 0;
}
        )";

        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstStatement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstStatement));

        auto& statementPtr = std::get<std::shared_ptr<Statement>>(firstStatement);
        REQUIRE(std::holds_alternative<WhileStatement>(*statementPtr));

        const WhileStatement& whileStatement = std::get<WhileStatement>(*statementPtr);

        REQUIRE(whileStatement.metadata.start.column == 1);
        REQUIRE(whileStatement.metadata.start.line == 1);
        REQUIRE(whileStatement.metadata.end.column == 2);
        REQUIRE(whileStatement.metadata.end.line == 3);

        const Expression& condition = *whileStatement.condition;
        REQUIRE(std::holds_alternative<BooleanLiteral>(condition));

        BooleanLiteral boolLiteral = std::get<BooleanLiteral>(condition);
        REQUIRE(boolLiteral.value == true);

        REQUIRE(whileStatement.block.size() == 1);

        const auto& blockElement = whileStatement.block[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(blockElement));

        const auto& assignmentPtr = std::get<std::shared_ptr<Statement>>(blockElement);
        REQUIRE(std::holds_alternative<VariableAssignment>(*assignmentPtr));

        const VariableAssignment& assignment = std::get<VariableAssignment>(*assignmentPtr);
        REQUIRE(assignment.identifier == "x");

        const Expression& valueExpr = *assignment.value.value();
        REQUIRE(std::holds_alternative<IntLiteral>(valueExpr));

        IntLiteral intLiteral = std::get<IntLiteral>(valueExpr);
        REQUIRE(intLiteral.value == 0);
    }

    SECTION("'for' loops are parsed properly") {
        std::string sourceCode = R"(for (mut int i = 0; i < 10; i = i + 1) {
    x = i;
}
        )";

        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstElement));

        auto& statementPtr = std::get<std::shared_ptr<Statement>>(firstElement);
        REQUIRE(std::holds_alternative<ForStatement>(*statementPtr));

        const ForStatement& forStmt = std::get<ForStatement>(*statementPtr);

        REQUIRE(forStmt.metadata.start.column == 1);
        REQUIRE(forStmt.metadata.start.line == 1);
        REQUIRE(forStmt.metadata.end.column == 2);
        REQUIRE(forStmt.metadata.end.line == 3);

        /**
         * VARIABLE DECLARATION
         */

        REQUIRE(forStmt.initialization.has_value());
        const Statement& initializationStatement = *forStmt.initialization.value();

        REQUIRE(std::holds_alternative<VariableDeclaration>(initializationStatement));
        const VariableDeclaration& variableDeclaration = std::get<VariableDeclaration>(initializationStatement);

        REQUIRE(variableDeclaration.isMutable == true);
        REQUIRE(variableDeclaration.identifier == "i");
        REQUIRE(variableDeclaration.type == "int");

        REQUIRE(variableDeclaration.value.has_value());
        const Expression& initExpr = *variableDeclaration.value.value();

        REQUIRE(std::holds_alternative<IntLiteral>(initExpr));
        const IntLiteral& initLiteral = std::get<IntLiteral>(initExpr);
        REQUIRE(initLiteral.value == 0);

        /**
         * CONDITION
         */

        REQUIRE(forStmt.condition.has_value());
        const auto& conditionExpr = forStmt.condition.value();
        REQUIRE(std::holds_alternative<BinaryOperation>(*conditionExpr));

        const BinaryOperation& binaryOp = std::get<BinaryOperation>(*conditionExpr);
        REQUIRE(binaryOp.op == "<");

        const Expression& lhs = *binaryOp.lhs;
        REQUIRE(std::holds_alternative<Identifier>(lhs));
        const Identifier& lhsId = std::get<Identifier>(lhs);
        REQUIRE(lhsId.name == "i");

        const Expression& rhs = *binaryOp.rhs;
        REQUIRE(std::holds_alternative<IntLiteral>(rhs));
        const IntLiteral& rhsLiteral = std::get<IntLiteral>(rhs);
        REQUIRE(rhsLiteral.value == 10);

        /**
         * UPDATE
         */

        REQUIRE(forStmt.update.has_value());
        const Statement& updateStmt = *forStmt.update.value();
        REQUIRE(std::holds_alternative<VariableAssignment>(updateStmt));

        const VariableAssignment& varAssign = std::get<VariableAssignment>(updateStmt);
        REQUIRE(varAssign.identifier == "i");

        REQUIRE(varAssign.value.has_value());
        const Expression& updateExpr = *varAssign.value.value();
        REQUIRE(std::holds_alternative<BinaryOperation>(updateExpr));

        const BinaryOperation& updateBinaryOp = std::get<BinaryOperation>(updateExpr);
        REQUIRE(updateBinaryOp.op == "+");

        const Expression& updateLhs = *updateBinaryOp.lhs;
        REQUIRE(std::holds_alternative<Identifier>(updateLhs));
        REQUIRE(std::get<Identifier>(updateLhs).name == "i");

        const Expression& updateRhs = *updateBinaryOp.rhs;
        REQUIRE(std::holds_alternative<IntLiteral>(updateRhs));
        REQUIRE(std::get<IntLiteral>(updateRhs).value == 1);

        /**
         * BLOCK
         */

        REQUIRE(forStmt.block.size() == 1);
        const auto& blockStmt = forStmt.block[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(blockStmt));

        const Statement& blockStatement = *std::get<std::shared_ptr<Statement>>(blockStmt);
        REQUIRE(std::holds_alternative<VariableAssignment>(blockStatement));

        const VariableAssignment& blockAssignment = std::get<VariableAssignment>(blockStatement);
        REQUIRE(blockAssignment.identifier == "x");

        REQUIRE(blockAssignment.value.has_value());
        const Expression& blockExpr = *blockAssignment.value.value();
        REQUIRE(std::holds_alternative<Identifier>(blockExpr));
        REQUIRE(std::get<Identifier>(blockExpr).name == "i");
    }

    SECTION("'break' statements are parsed properly") {
        std::string sourceCode = R"(while (true) {
    break;
}
        )";

        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstElement));

        auto& statementPtr = std::get<std::shared_ptr<Statement>>(firstElement);
        REQUIRE(std::holds_alternative<WhileStatement>(*statementPtr));

        const WhileStatement& whileStmt = std::get<WhileStatement>(*statementPtr);
        REQUIRE(whileStmt.block.size() == 1);

        REQUIRE(whileStmt.metadata.start.column == 1);
        REQUIRE(whileStmt.metadata.start.line == 1);
        REQUIRE(whileStmt.metadata.end.column == 2);
        REQUIRE(whileStmt.metadata.end.line == 3);

        const auto& blockElement = whileStmt.block[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(blockElement));

        const auto& breakStmtPtr = std::get<std::shared_ptr<Statement>>(blockElement);
        REQUIRE(std::holds_alternative<BreakStatement>(*breakStmtPtr));
    }

    SECTION("'continue' statements are parsed properly") {
        std::string sourceCode = R"(while (true) {
    continue;
}
        )";

        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(sourceCode, tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);
        auto& firstElement = program.body[0];

        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstElement));
        auto& statementPtr = std::get<std::shared_ptr<Statement>>(firstElement);

        REQUIRE(std::holds_alternative<WhileStatement>(*statementPtr));
        const WhileStatement& whileStmt = std::get<WhileStatement>(*statementPtr);
        REQUIRE(whileStmt.block.size() == 1);

        REQUIRE(whileStmt.metadata.start.column == 1);
        REQUIRE(whileStmt.metadata.start.line == 1);
        REQUIRE(whileStmt.metadata.end.column == 2);
        REQUIRE(whileStmt.metadata.end.line == 3);

        const auto& blockElement = whileStmt.block[0];
        REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(blockElement));

        const auto& continueStmtPtr = std::get<std::shared_ptr<Statement>>(blockElement);
        REQUIRE(std::holds_alternative<ContinueStatement>(*continueStmtPtr));
    }

    SECTION("'return' statements are parsed properly") {
        SECTION("'return' statements without expression are parsed properly") {
            std::string sourceCode = "return;";

            Lexer lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            REQUIRE(program.body.size() == 1);

            auto& firstElement = program.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstElement));

            auto& statementPtr = std::get<std::shared_ptr<Statement>>(firstElement);
            REQUIRE(std::holds_alternative<ReturnStatement>(*statementPtr));

            const ReturnStatement& returnStmt = std::get<ReturnStatement>(*statementPtr);
            REQUIRE(!returnStmt.expression.has_value());

            REQUIRE(returnStmt.metadata.start.column == 1);
            REQUIRE(returnStmt.metadata.start.line == 1);
            REQUIRE(returnStmt.metadata.end.column == 8);
            REQUIRE(returnStmt.metadata.end.line == 1);
        }

        SECTION("'return' statements with expression are parsed properly") {
            std::string sourceCode = "return 69;";

            Lexer lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            REQUIRE(program.body.size() == 1);

            auto& firstElement = program.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(firstElement));

            auto& statementPtr = std::get<std::shared_ptr<Statement>>(firstElement);
            REQUIRE(std::holds_alternative<ReturnStatement>(*statementPtr));

            const ReturnStatement& returnStmt = std::get<ReturnStatement>(*statementPtr);
            REQUIRE(returnStmt.expression.has_value());

            REQUIRE(returnStmt.metadata.start.column == 1);
            REQUIRE(returnStmt.metadata.start.line == 1);
            REQUIRE(returnStmt.metadata.end.column == 11);
            REQUIRE(returnStmt.metadata.end.line == 1);

            const auto& exprPtr = returnStmt.expression.value();
            REQUIRE(std::holds_alternative<IntLiteral>(*exprPtr));

            const IntLiteral& intLiteral = std::get<IntLiteral>(*exprPtr);
            REQUIRE(intLiteral.value == 69);
        }
    }

    SECTION("Function definitions are parsed properly") {
        SECTION("Function with no parameters is parsed properly") {
            std::string sourceCode = R"(fn string greet() {
    return "Hello, World!";
}
            )";

            Lexer lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            REQUIRE(program.body.size() == 1);

            auto& firstElement = program.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

            auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
            REQUIRE(std::holds_alternative<Function>(*expressionPtr));

            const Function& functionDef = std::get<Function>(*expressionPtr);
            REQUIRE(functionDef.name == "greet");
            REQUIRE(functionDef.returnType == "string");
            REQUIRE(functionDef.parameters.size() == 0);

            REQUIRE(functionDef.body.size() == 1);

            REQUIRE(functionDef.metadata.start.column == 1);
            REQUIRE(functionDef.metadata.start.line == 1);
            REQUIRE(functionDef.metadata.end.column == 2);
            REQUIRE(functionDef.metadata.end.line == 3);

            const auto& returnStmt = functionDef.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(returnStmt));

            const auto& returnPtr = std::get<std::shared_ptr<Statement>>(returnStmt);
            REQUIRE(std::holds_alternative<ReturnStatement>(*returnPtr));

            const ReturnStatement& retStmt = std::get<ReturnStatement>(*returnPtr);
            REQUIRE(retStmt.expression.has_value());

            const auto& exprPtr = retStmt.expression.value();
            REQUIRE(std::holds_alternative<StringLiteral>(*exprPtr));

            const StringLiteral& strLiteral = std::get<StringLiteral>(*exprPtr);
            REQUIRE(strLiteral.value == "Hello, World!");
        }

        SECTION("Function with multiple parameters is parsed properly") {
            std::string sourceCode = R"(fn int add(const int a, const int b) {
    return a + b;
}
            )";

            Lexer lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            REQUIRE(program.body.size() == 1);

            auto& firstElement = program.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

            auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
            REQUIRE(std::holds_alternative<Function>(*expressionPtr));

            const Function& functionDef = std::get<Function>(*expressionPtr);
            REQUIRE(functionDef.name == "add");
            REQUIRE(functionDef.returnType == "int");

            REQUIRE(functionDef.parameters.size() == 2);

            REQUIRE(functionDef.metadata.start.column == 1);
            REQUIRE(functionDef.metadata.start.line == 1);
            REQUIRE(functionDef.metadata.end.column == 2);
            REQUIRE(functionDef.metadata.end.line == 3);

            /**
             * FIRST PARAMETER
             */

            REQUIRE(functionDef.parameters[0]->isMutable == false);
            REQUIRE(functionDef.parameters[0]->identifier == "a");
            REQUIRE(functionDef.parameters[0]->type == "int");

            /**
             * SECOND PARAMETER
             */

            REQUIRE(functionDef.parameters[1]->isMutable == false);
            REQUIRE(functionDef.parameters[1]->identifier == "b");
            REQUIRE(functionDef.parameters[1]->type == "int");

            REQUIRE(functionDef.body.size() == 1);

            const auto& returnStmt = functionDef.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(returnStmt));

            const auto& returnPtr = std::get<std::shared_ptr<Statement>>(returnStmt);
            REQUIRE(std::holds_alternative<ReturnStatement>(*returnPtr));

            const ReturnStatement& retStmt = std::get<ReturnStatement>(*returnPtr);
            REQUIRE(retStmt.expression.has_value());

            const auto& exprPtr = retStmt.expression.value();
            REQUIRE(std::holds_alternative<BinaryOperation>(*exprPtr));

            const BinaryOperation& binOp = std::get<BinaryOperation>(*exprPtr);

            REQUIRE(std::holds_alternative<Identifier>(*binOp.lhs));
            REQUIRE(std::get<Identifier>(*binOp.lhs).name == "a");

            REQUIRE(binOp.op == "+");

            REQUIRE(std::holds_alternative<Identifier>(*binOp.rhs));
            REQUIRE(std::get<Identifier>(*binOp.rhs).name == "b");
        }

        SECTION("Function with multiple parameters and some default values is parsed properly") {
            std::string sourceCode = R"(fn int multiply(const int a = 2, const int b = 3) {
    return a * b;
}
            )";

            Lexer lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            REQUIRE(program.body.size() == 1);

            auto& firstElement = program.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

            auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
            REQUIRE(std::holds_alternative<Function>(*expressionPtr));

            const Function& functionDef = std::get<Function>(*expressionPtr);
            REQUIRE(functionDef.name == "multiply");
            REQUIRE(functionDef.returnType == "int");

            REQUIRE(functionDef.parameters.size() == 2);

            REQUIRE(functionDef.metadata.start.column == 1);
            REQUIRE(functionDef.metadata.start.line == 1);
            REQUIRE(functionDef.metadata.end.column == 2);
            REQUIRE(functionDef.metadata.end.line == 3);

            /**
             * FIRST PARAMETER
             */

            REQUIRE(functionDef.parameters[0]->isMutable == false);
            REQUIRE(functionDef.parameters[0]->identifier == "a");
            REQUIRE(functionDef.parameters[0]->type == "int");
            REQUIRE(functionDef.parameters[0]->value.has_value());

            const auto& defaultValueA = functionDef.parameters[0]->value.value();
            REQUIRE(std::holds_alternative<IntLiteral>(*defaultValueA));

            IntLiteral defaultIntLiteralA = std::get<IntLiteral>(*defaultValueA);
            REQUIRE(defaultIntLiteralA.value == 2);

            /**
             * SECOND PARAMETER
             */

            REQUIRE(functionDef.parameters[1]->isMutable == false);
            REQUIRE(functionDef.parameters[1]->identifier == "b");
            REQUIRE(functionDef.parameters[1]->type == "int");
            REQUIRE(functionDef.parameters[1]->value.has_value());

            const auto& defaultValueB = functionDef.parameters[1]->value.value();
            REQUIRE(std::holds_alternative<IntLiteral>(*defaultValueB));

            IntLiteral defaultIntLiteralB = std::get<IntLiteral>(*defaultValueB);
            REQUIRE(defaultIntLiteralB.value == 3);

            REQUIRE(functionDef.body.size() == 1);

            const auto& returnStmt = functionDef.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Statement>>(returnStmt));

            const auto& returnPtr = std::get<std::shared_ptr<Statement>>(returnStmt);
            REQUIRE(std::holds_alternative<ReturnStatement>(*returnPtr));

            const ReturnStatement& retStmt = std::get<ReturnStatement>(*returnPtr);
            REQUIRE(retStmt.expression.has_value());

            const auto& exprPtr = retStmt.expression.value();
            REQUIRE(std::holds_alternative<BinaryOperation>(*exprPtr));

            const BinaryOperation& binOp = std::get<BinaryOperation>(*exprPtr);

            REQUIRE(std::holds_alternative<Identifier>(*binOp.lhs));
            REQUIRE(std::get<Identifier>(*binOp.lhs).name == "a");

            REQUIRE(binOp.op == "*");

            REQUIRE(std::holds_alternative<Identifier>(*binOp.rhs));
            REQUIRE(std::get<Identifier>(*binOp.rhs).name == "b");
        }
    }

    SECTION("Function calls are parsed properly") {
        SECTION("Function call with no arguments is parsed properly") {
            std::string sourceCode = "greet()";

            Lexer lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            REQUIRE(program.body.size() == 1);

            auto& firstElement = program.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

            auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
            REQUIRE(std::holds_alternative<FunctionCall>(*expressionPtr));

            const FunctionCall& functionCall = std::get<FunctionCall>(*expressionPtr);
            REQUIRE(functionCall.identifier == "greet");

            REQUIRE(functionCall.arguments.empty() == true);

            REQUIRE(functionCall.metadata.start.column == 1);
            REQUIRE(functionCall.metadata.start.line == 1);
            REQUIRE(functionCall.metadata.end.column == 8);
            REQUIRE(functionCall.metadata.end.line == 1);
        }

        SECTION("Function call with multiple arguments is parsed properly") {
            std::string sourceCode = "add(69, 3.14, \"hello\")";

            Lexer lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            REQUIRE(program.body.size() == 1);

            auto& firstElement = program.body[0];
            REQUIRE(std::holds_alternative<std::shared_ptr<Expression>>(firstElement));

            auto& expressionPtr = std::get<std::shared_ptr<Expression>>(firstElement);
            REQUIRE(std::holds_alternative<FunctionCall>(*expressionPtr));

            const FunctionCall& functionCall = std::get<FunctionCall>(*expressionPtr);
            REQUIRE(functionCall.identifier == "add");

            REQUIRE(functionCall.arguments.size() == 3);

            REQUIRE(functionCall.metadata.start.column == 1);
            REQUIRE(functionCall.metadata.start.line == 1);
            REQUIRE(functionCall.metadata.end.column == 23);
            REQUIRE(functionCall.metadata.end.line == 1);

            REQUIRE(std::holds_alternative<IntLiteral>(*functionCall.arguments[0]));
            REQUIRE(std::get<IntLiteral>(*functionCall.arguments[0]).value == 69);

            REQUIRE(std::holds_alternative<FloatLiteral>(*functionCall.arguments[1]));
            REQUIRE(std::get<FloatLiteral>(*functionCall.arguments[1]).value == 3.14f);

            REQUIRE(std::holds_alternative<StringLiteral>(*functionCall.arguments[2]));
            REQUIRE(std::get<StringLiteral>(*functionCall.arguments[2]).value == "hello");
        }
    }
}
