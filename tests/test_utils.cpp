#include <stdexcept>
#include "utils.hpp"
#include "catch_amalgamated.hpp"

TEST_CASE("Files can be read", "[file]") {
    SECTION("Reading files") {
        SECTION("Reading the contents of an existing file") {
            std::string contents = readFile("./tests/sample_code/arithmetic.nv");

            REQUIRE(contents == "1 + 2;\n");
        }

        SECTION("Throws an error for a missing file") {
            bool hasSucceeded = true;
            try {
                std::string contents = readFile("./tests/sample_code/arithmetic1.nv");
            } catch (const std::runtime_error& e) {
                hasSucceeded = false;
            }

            REQUIRE(hasSucceeded == false);
        }
    }

    SECTION("Splits strings by newline characters") {
        SECTION("Single line input") {
            std::string input = "Hello";
            std::vector<std::string> result = splitStringByNewline(input);

            REQUIRE(result.size() == 1);

            REQUIRE(result[0] == "Hello");
        }

        SECTION("Multiple lines input") {
            std::string input = "Hello\nWorld\n!";
            std::vector<std::string> result = splitStringByNewline(input);

            REQUIRE(result.size() == 3);

            REQUIRE(result[0] == "Hello");
            REQUIRE(result[1] == "World");
            REQUIRE(result[2] == "!");
        }

        SECTION("Trailing newline") {
            std::string input = "Hello\n";
            std::vector<std::string> result = splitStringByNewline(input);

            REQUIRE(result.size() == 2);

            REQUIRE(result[0] == "Hello");
            REQUIRE(result[1] == "");
        }

        SECTION("Empty string") {
            std::string input = "";
            std::vector<std::string> result = splitStringByNewline(input);

            REQUIRE(result.empty());
        }
    }
}
