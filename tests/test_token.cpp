#include <catch2/catch_test_macros.hpp>
#include "token.hpp"
#include "ast.hpp"

TEST_CASE("TokenMetadata works as expected", "[token]") {
    SECTION("TokenMetadata works as expected") {
        SECTION("Default constructor initializes correctly") {
            TokenMetadata metadata;

            REQUIRE(metadata.column == 0);
            REQUIRE(metadata.line == 0);
            REQUIRE(metadata.length == 0);
        }

        SECTION("Parameterized constructor initializes correctly") {
            TokenMetadata metadata(5, 10, 15);

            REQUIRE(metadata.column == 5);
            REQUIRE(metadata.line == 10);
            REQUIRE(metadata.length == 15);
        }

        SECTION("toStartPosition returns correct NodePosition") {
            TokenMetadata metadata(5, 10, 15);
            NodePosition start = metadata.toStartPosition();

            REQUIRE(start.column == 5);
            REQUIRE(start.line == 10);
        }

        SECTION("toEndPosition returns correct NodePosition") {
            TokenMetadata metadata(5, 10, 15);
            NodePosition end = metadata.toEndPosition();

            REQUIRE(end.column == 20);
            REQUIRE(end.line == 10);
        }
    }

    SECTION("Token works as expected") {
        SECTION("Default constructor initializes correctly") {
            Token token;
            REQUIRE(token.type == TokenType::INTEGER);
            REQUIRE(token.value.empty());
            REQUIRE(token.metadata.column == 0);
            REQUIRE(token.metadata.line == 0);
            REQUIRE(token.metadata.length == 0);
        }

        SECTION("Parameterized constructor initializes correctly") {
            TokenMetadata metadata(5, 10, 15);
            Token token(TokenType::STRING, "hello", metadata);
            REQUIRE(token.type == TokenType::STRING);
            REQUIRE(token.value == "hello");
            REQUIRE(token.metadata.column == 5);
            REQUIRE(token.metadata.line == 10);
            REQUIRE(token.metadata.length == 15);
        }
    }

    SECTION("getTokenTypeString works as expected") {
        SECTION("Returns correct string for valid TokenType") {
            REQUIRE(getTokenTypeString(TokenType::STRING) == "String");
            REQUIRE(getTokenTypeString(TokenType::INTEGER) == "Integer");
            REQUIRE(getTokenTypeString(TokenType::BOOLEAN) == "Boolean");
        }

        SECTION("Throws exception for invalid TokenType") {
            REQUIRE_THROWS_AS(getTokenTypeString(static_cast<TokenType>(-1)), std::out_of_range);
        }
    }
}
