#include <iostream>
#include <string>
#include "file.hpp"
#include "lexer.hpp"

int main()
{
    std::string contents = readFile("./tests/sample_code/arithmetic.nv");

    Lexer lexer = Lexer(contents);
    std::vector<Token> tokens = lexer.tokenize();

    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << tokens[i].value << std::endl;
    }

    return 0;
}
