#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <string>
#include "utils.hpp"

TEST_CASE("Files can be read", "[file]") {
    SECTION("Reading files") {
        SECTION("Reading the contents of an existing file") {
            std::string contents = readFile("../tests/sample_code/arithmetic.nv");

            REQUIRE(contents == "1 + 2;\n");
        }

        SECTION("Throws an error for a missing file") {
            bool hasSucceeded = true;
            try {
                std::string contents = readFile("./tests/sample_code/arithmetic1.nv");
            } catch (...) {
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

    SECTION("Parsing command line arguments works correctly") {
        /**
         * ? Convert std::vector<std::string> to char**
         * * See https://stackoverflow.com/a/26032303
         */

         /**
          * ? Why static_cast<int>
          * * Because parseCommandLineArguments expects an int (argc), but cstrings.size() returns a size_t.
          */

        SECTION("No arguments returns an empty map") {
            std::vector<std::string> strings = { "nv" };
            std::vector<char*> cstrings;
            cstrings.reserve(strings.size());

            for (const auto& str : strings) {
                cstrings.push_back(const_cast<char*>(str.c_str()));
            }

            auto args = parseCommandLineArguments(static_cast<int>(cstrings.size()), &cstrings[0]);

            REQUIRE(args.empty());
        }

        SECTION("Single argument is parsed correctly") {
            std::vector<std::string> strings = { "nv", "--arg=true", "file.nv" };
            std::vector<char*> cstrings;
            cstrings.reserve(strings.size());

            for (const auto& str : strings) {
                cstrings.push_back(const_cast<char*>(str.c_str()));
            }

            auto args = parseCommandLineArguments(static_cast<int>(cstrings.size()), &cstrings[0]);

            REQUIRE(args.size() == 2);

            REQUIRE(args["--arg"] == "true");
            REQUIRE(args["filename"] == "file.nv");
        }

        SECTION("Arguments are parsed correctly") {
            std::vector<std::string> strings = { "nv", "--arg1=true", "--arg2=false", "file.nv" };
            std::vector<char*> cstrings;
            cstrings.reserve(strings.size());

            for (const auto& str : strings) {
                cstrings.push_back(const_cast<char*>(str.c_str()));
            }

            auto args = parseCommandLineArguments(static_cast<int>(cstrings.size()), &cstrings[0]);

            REQUIRE(args.size() == 3);

            REQUIRE(args["--arg1"] == "true");
            REQUIRE(args["--arg2"] == "false");
            REQUIRE(args["filename"] == "file.nv");
        }

        SECTION("Arguments that don't start with -- are ignored") {
            std::vector<std::string> strings = { "nv", "-arg1=true", "--arg2=false", "file.nv" };
            std::vector<char*> cstrings;
            cstrings.reserve(strings.size());

            for (const auto& str : strings) {
                cstrings.push_back(const_cast<char*>(str.c_str()));
            }

            auto args = parseCommandLineArguments(static_cast<int>(cstrings.size()), &cstrings[0]);

            REQUIRE(args.size() == 2);

            REQUIRE(args["--arg2"] == "false");
            REQUIRE(args["filename"] == "file.nv");
        }
    }

    SECTION("padRight works correctly") {
        SECTION("An empty string is filled properly") {
            std::string received = padRight("", 10, ' ');
            std::string expected = "          ";
            REQUIRE(received == expected);
        }

        SECTION("A string shorter than the total width is filled properly") {
            std::string received = padRight("Hello", 10, ' ');
            std::string expected = "Hello     ";

            REQUIRE(received == expected);
        }

        SECTION("A string equal to the total width is not modified") {
            std::string received = padRight("Hello", 5, ' ');
            std::string expected = "Hello";

            REQUIRE(received == expected);
        }

        SECTION("A string longer than the total width is not modified") {
            std::string received = padRight("Hello World", 10, ' ');
            std::string expected = "Hello World";

            REQUIRE(received == expected);
        }

        SECTION("Using a different fill char works correctly") {
            std::string received = padRight("Hello", 10, '*');
            std::string expected = "Hello*****";

            REQUIRE(received == expected);
        }

        SECTION("Using the default fill char works correctly") {
            std::string received = padRight("Hello", 10);
            std::string expected = "Hello     ";

            REQUIRE(received == expected);
        }
    }
}
