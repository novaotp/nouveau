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
        REQUIRE(std::holds_alternative<StringLiteral>(*expressionPtr));

        StringLiteral stringLiteral = std::get<StringLiteral>(*expressionPtr);
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
        REQUIRE(std::holds_alternative<IntLiteral>(*expressionPtr));

        IntLiteral intLiteral = std::get<IntLiteral>(*expressionPtr);
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
        REQUIRE(std::holds_alternative<FloatLiteral>(*expressionPtr));

        FloatLiteral floatLiteral = std::get<FloatLiteral>(*expressionPtr);
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
        REQUIRE(std::holds_alternative<BooleanLiteral>(*expressionPtr));

        BooleanLiteral booleanLiteral = std::get<BooleanLiteral>(*expressionPtr);
        REQUIRE(booleanLiteral.value == true);
    }

    SECTION("Vectors are parsed properly") {
        std::string sourceCode = "[69, false, \"hello\", 3.14]";
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<Vector>(*expressionPtr));

        const Vector& vector = std::get<Vector>(*expressionPtr);

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

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));
        BinaryOperation arithmeticOperation = std::move(std::get<BinaryOperation>(*expressionPtr));

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

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        // Top-level ((420 + (69 * 3.14)) - 7)
        BinaryOperation topLevelOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(topLevelOperation.op == "-");

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

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        // Top-level ((420 + 69) * (3.14 - 7))
        BinaryOperation topLevelOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(topLevelOperation.op == "*");

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

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));
        BinaryOperation comparisonOperation = std::move(std::get<BinaryOperation>(*expressionPtr));

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

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<LogicalNotOperation>(*expressionPtr));

        LogicalNotOperation notOperation = std::move(std::get<LogicalNotOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<BooleanLiteral>(*notOperation.expression));

        BooleanLiteral boolLiteral = std::get<BooleanLiteral>(*notOperation.expression);
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
        REQUIRE(std::holds_alternative<BooleanLiteral>(*andOperation.lhs));

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

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstElement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(firstElement));

        auto& expressionPtr = std::get<std::unique_ptr<Expression>>(firstElement);
        REQUIRE(std::holds_alternative<BinaryOperation>(*expressionPtr));

        BinaryOperation andOperation = std::move(std::get<BinaryOperation>(*expressionPtr));
        REQUIRE(std::holds_alternative<BooleanLiteral>(*andOperation.lhs));

        BooleanLiteral lhsBooleanLiteral = std::get<BooleanLiteral>(*andOperation.lhs);
        REQUIRE(lhsBooleanLiteral.value == false);

        REQUIRE(andOperation.op == "||");

        auto& rhs = andOperation.rhs;
        REQUIRE(std::holds_alternative<BooleanLiteral>(*rhs));

        BooleanLiteral rhsBooleanLiteral = std::get<BooleanLiteral>(*rhs);
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
        REQUIRE(std::holds_alternative<StringLiteral>(*valueExpressionPtr));

        StringLiteral stringLiteral = std::get<StringLiteral>(*valueExpressionPtr);
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
        REQUIRE(std::holds_alternative<StringLiteral>(*valueExpressionPtr));

        StringLiteral stringLiteral = std::get<StringLiteral>(*valueExpressionPtr);
        REQUIRE(stringLiteral.value == "Hello, World !");
    }

    SECTION("If-else conditions are parsed properly") {
        std::string sourceCode = R"(
            if (true) {
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

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(program.body[0]));
        auto& mainStatement = std::get<std::unique_ptr<Statement>>(program.body[0]);

        REQUIRE(std::holds_alternative<IfStatement>(*mainStatement));
        const IfStatement& ifStatement = std::get<IfStatement>(*mainStatement);

        SECTION("Validate the main if condition") {
            REQUIRE(std::holds_alternative<BooleanLiteral>(*ifStatement.condition));
            REQUIRE(std::get<BooleanLiteral>(*ifStatement.condition).value == true);
        }

        SECTION("Validate the thenBlock") {
            REQUIRE(ifStatement.thenBlock.size() == 2);

            REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(ifStatement.thenBlock[0]));
            auto& thenStmt1 = std::get<std::unique_ptr<Statement>>(ifStatement.thenBlock[0]);

            REQUIRE(std::holds_alternative<VariableAssignment>(*thenStmt1));
            REQUIRE(std::get<VariableAssignment>(*thenStmt1).identifier == "x");

            REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(ifStatement.thenBlock[1]));
            auto& thenExpr2 = std::get<std::unique_ptr<Expression>>(ifStatement.thenBlock[1]);

            REQUIRE(std::holds_alternative<StringLiteral>(*thenExpr2));
            REQUIRE(std::get<StringLiteral>(*thenExpr2).value == "hello");
        }

        SECTION("Validate the elseifClauses") {
            REQUIRE(ifStatement.elseifClauses.size() == 1);

            REQUIRE(std::holds_alternative<BooleanLiteral>(*ifStatement.elseifClauses[0].first));
            REQUIRE(std::get<BooleanLiteral>(*ifStatement.elseifClauses[0].first).value == false);

            REQUIRE(ifStatement.elseifClauses[0].second.size() == 1);

            REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(ifStatement.elseifClauses[0].second[0]));
            auto& elseifStmt = std::get<std::unique_ptr<Statement>>(ifStatement.elseifClauses[0].second[0]);

            REQUIRE(std::holds_alternative<VariableAssignment>(*elseifStmt));
            REQUIRE(std::get<VariableAssignment>(*elseifStmt).identifier == "y");
        }

        SECTION("Validate the elseBlock") {
            REQUIRE(ifStatement.elseBlock.size() == 2);

            REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(ifStatement.elseBlock[0]));
            auto& elseStmt1 = std::get<std::unique_ptr<Statement>>(ifStatement.elseBlock[0]);

            REQUIRE(std::holds_alternative<VariableDeclaration>(*elseStmt1));
            REQUIRE(std::get<VariableDeclaration>(*elseStmt1).identifier == "z");

            REQUIRE(std::holds_alternative<std::unique_ptr<Expression>>(ifStatement.elseBlock[1]));
            auto& elseExpr2 = std::get<std::unique_ptr<Expression>>(ifStatement.elseBlock[1]);

            REQUIRE(std::holds_alternative<NullLiteral>(*elseExpr2));
        }
    }

    SECTION("While loops are parsed properly") {
        std::string sourceCode = R"(
        while (true) {
            x = 0;
        }
    )";

        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        Program program = parser.parse();

        REQUIRE(program.body.size() == 1);

        auto& firstStatement = program.body[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(firstStatement));

        auto& statementPtr = std::get<std::unique_ptr<Statement>>(firstStatement);
        REQUIRE(std::holds_alternative<WhileStatement>(*statementPtr));

        const WhileStatement& whileStatement = std::get<WhileStatement>(*statementPtr);

        const Expression& condition = *whileStatement.condition;
        REQUIRE(std::holds_alternative<BooleanLiteral>(condition));

        BooleanLiteral boolLiteral = std::get<BooleanLiteral>(condition);
        REQUIRE(boolLiteral.value == true);

        REQUIRE(whileStatement.block.size() == 1);

        const auto& blockElement = whileStatement.block[0];
        REQUIRE(std::holds_alternative<std::unique_ptr<Statement>>(blockElement));

        const auto& assignmentPtr = std::get<std::unique_ptr<Statement>>(blockElement);
        REQUIRE(std::holds_alternative<VariableAssignment>(*assignmentPtr));

        const VariableAssignment& assignment = std::get<VariableAssignment>(*assignmentPtr);
        REQUIRE(assignment.identifier == "x");

        const Expression& valueExpr = *assignment.value.value();
        REQUIRE(std::holds_alternative<IntLiteral>(valueExpr));

        IntLiteral intLiteral = std::get<IntLiteral>(valueExpr);
        REQUIRE(intLiteral.value == 0);
    }

}
