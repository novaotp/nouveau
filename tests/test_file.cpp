#include <stdexcept>
#include "file.hpp"
#include "catch_amalgamated.hpp"

TEST_CASE("Files can be read", "[file]") {
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
