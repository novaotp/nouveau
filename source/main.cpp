#include <iostream>
#include <string>
#include "file.h"

int main()
{
    std::string contents;
    try
    {
        contents = readFile("./tests/sample_code/arithmetic.flux");
        std::cout << contents;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
