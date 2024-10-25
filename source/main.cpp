#include <iostream>
#include <string>
#include "file.hpp"
#include "lexer.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: Please provide the path to the source file." << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::string contents = readFile(filePath);

    Lexer lexer = Lexer(contents);
    std::vector<Token> tokens = lexer.tokenize();

    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << tokens[i].value << std::endl;
    }

    return 0;
}
