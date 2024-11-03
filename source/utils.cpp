#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "utils.hpp"

std::string readFile(const std::string& path) {
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

std::vector<std::string> splitStringByNewline(const std::string& str) {
    std::vector<std::string> result;
    std::istringstream stream(str);
    std::string line;

    while (std::getline(stream, line)) {
        result.push_back(line);
    }

    if (!str.empty() && str.back() == '\n') {
        result.push_back(""); // Empty string for trailing newline
    }

    return result;
};
