#ifndef UTILS_HPP
#define UTILS_HPP

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW   "\033[33m"

#include <vector>
#include <string>
#include <map>

std::string readFile(const std::string& path);
std::vector<std::string> splitStringByNewline(const std::string& str);
std::string padRight(const std::string& input, size_t totalWidth, char fillChar = ' ');

/// @brief Parses the command line arguments.
/// @return The parsed command line arguments as a map of name-value pairs.
/// @note 1. The first argument is ignored as it is the name of the executable.
/// @note 2. If the last argument doesn't have a '=', it will be treated as the main filename.
/// @note 3. Any argument that doesn't start with '--' will be ignored.
std::map<std::string, std::string> parseCommandLineArguments(int argc, char** argv);

#endif // UTILS_HPP
