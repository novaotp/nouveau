#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "file.hpp"

std::string readFile(const std::string path) {
    std::string contents;
    std::fstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open the file " + path);
    }

    std::string line;
    while (std::getline(file, line)) {
        contents += line + '\n';
    }

    return contents;
}
