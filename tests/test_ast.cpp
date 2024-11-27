#include <catch2/catch_test_macros.hpp>
#include "ast.hpp"

TEST_CASE("AST works properly", "[ast]") {
    SECTION("Node Types .toString() methods are correct") {
        REQUIRE(StringType().toString() == "string");
        REQUIRE(IntegerType().toString() == "int");
        REQUIRE(FloatType().toString() == "float");
        REQUIRE(BooleanType().toString() == "bool");
    }

    SECTION("NodeType equality works properly") {
        REQUIRE(StringType().compare(std::make_shared<StringType>()) == true);
        REQUIRE(StringType().compare(std::make_shared<IntegerType>()) == false);
    }
}
