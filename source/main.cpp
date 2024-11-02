#include <iostream>
#include <variant>
#include <string>
#include "file.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Please provide the path to the source file." << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::string contents = readFile(filePath);

    Lexer lexer(contents);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(contents, tokens);
    Program program;
    try {
        program = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    program.prettyPrint();

    return 0;
}
