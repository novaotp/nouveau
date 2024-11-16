#include <iostream>
#include <variant>
#include <string>
#include <vector>
#include "utils.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Please provide the path to the source file." << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::string sourceCode = readFile(filePath);

    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(sourceCode, tokens);
    Program program;
    try {
        program = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    // program.prettyPrint();

    Semer semer(sourceCode, program);
    const std::vector<SemerError>& errors = semer.analyze();

    if (!errors.empty()) {
        size_t warningCount = 0;
        size_t errorCount = 0;

        for (const auto& error : errors) {
            if (error.level == SemerErrorLevel::WARNING) {
                warningCount++;
            } else if (error.level == SemerErrorLevel::ERROR) {
                errorCount++;
            }
        }

        std::cout << std::endl;
        std::cout << "\tAfter compiling, found " + std::string(YELLOW) + std::to_string(warningCount) + " warning(s)" + RESET + " and " + RED + std::to_string(errorCount) + " error(s)" + RESET + "." << std::endl;

        for (const auto& error : errors) {
            std::cout << error.toString() << std::flush;
        }
    } else {
        std::cout << std::endl;
        std::cout << GREEN << "\tAnalyzed source code, no errors found" << RESET << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
