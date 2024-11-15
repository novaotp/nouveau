#include <vector>
#include <string>
#include "token.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semer.hpp"
#include "catch_amalgamated.hpp"

TEST_CASE("Semer works correctly", "[semer]") {
    SECTION("Variable declarations are analyzed properly") {
        SECTION("Constant variable declarations without initial values are flagged as WARNING") {
            std::string sourceCode = "bool is_valid;";
            Lexer lexer = Lexer(sourceCode);
            std::vector<Token> tokens = lexer.tokenize();

            Parser parser(sourceCode, tokens);
            Program program = parser.parse();

            Semer semer(sourceCode, program);
            const std::vector<SemerError>& errors = semer.analyze();

            REQUIRE(errors.size() == 1);

            const SemerError& error = errors.at(0);

            REQUIRE(error.type == SemerErrorType::SEMANTIC_ERROR);
            REQUIRE(error.level == SemerErrorLevel::WARNING);
        }
    }
}
