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

std::map<std::string, std::string> parseCommandLineArguments(int argc, char** argv) {
    std::map<std::string, std::string> result;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg.substr(0, 2) != "--") {
            continue;
        }

        std::string key;
        std::string value;

        // Parse the argument as key=value or key=true
        if (arg.find('=') == std::string::npos) {
            key = arg;
            value = "true";
        } else {
            key = arg.substr(0, arg.find('='));
            value = arg.substr(arg.find('=') + 1);
        }

        result[key] = value;
    }

    // Add the last argument as "filename" if it's not a key-value pair
    if (argc > 1 && std::string(argv[argc - 1]).substr(0, 2) != "--" && std::string(argv[argc - 1]).find('=') == std::string::npos) {
        result["filename"] = argv[argc - 1];
    }

    return result;
}

std::string padRight(const std::string& input, size_t totalWidth, char fillChar) {
    if (input.size() >= totalWidth) {
        return input;
    }

    return input + std::string(totalWidth - input.size(), fillChar);
}
