#ifndef UTILS_HPP
#define UTILS_HPP

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW   "\033[33m"

#include <vector>
#include <string>

std::string readFile(const std::string& path);
std::vector<std::string> splitStringByNewline(const std::string& str);

#endif // UTILS_HPP
