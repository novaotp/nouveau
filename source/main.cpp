#include <iostream>
#include <variant>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include "config.h"
#include "utils.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "semer.hpp"
#include "constant_folding.hpp"

int compile(std::map<std::string, std::string> commandLineArguments) {
    std::filesystem::path currentPath = std::filesystem::current_path();

    std::string filePath = commandLineArguments["filename"];
    std::string absoluteFilePath = std::filesystem::canonical(currentPath / filePath).string();

    std::string sourceCode = readFile(filePath);

    std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::system_clock::now().time_since_epoch()
                                      );

    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    std::chrono::milliseconds lexerEnd = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
                                         );

    Parser parser(sourceCode, tokens);
    Program program;
    try {
        program = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    std::chrono::milliseconds parserEnd = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
                                          );

    // program.prettyPrint();

    Semer semer(sourceCode, program, absoluteFilePath);
    auto [errors, scope] = semer.analyze();

    std::chrono::milliseconds semerEnd = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
                                         );

    if (!errors.empty()) {
        for (const auto& error : errors) {
            std::cout << error.toString() << std::flush;
        }

        size_t warningCount = 0;
        size_t errorCount = 0;

        for (const auto& error : errors) {
            if (error.level == SemerErrorLevel::WARNING) {
                warningCount++;
            } else if (error.level == SemerErrorLevel::ERROR) {
                errorCount++;
            }
        }

        std::cout << std::string(8, ' ') << std::string(45, '-') << "\n" << std::endl;
        std::cout << "\tAfter compiling, found " + std::string(YELLOW) + std::to_string(warningCount) + " warning(s)" + RESET + " and " + RED + std::to_string(errorCount) + " error(s)" + RESET + ".\n" << std::endl;

        // Breaking early to avoid continuing with errors
        if (errorCount > 0) return 1;
    } else {
        std::cout << GREEN << "\n\tAnalyzed source code, no errors found.\n" << RESET << std::endl;
    }

    ConstantFolder constantFolder(program, scope);
    constantFolder.optimize().prettyPrint();

    std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(
                                        std::chrono::system_clock::now().time_since_epoch()
                                    );

    if (commandLineArguments.find("--timings") != commandLineArguments.end() && commandLineArguments["--timings"] == "true") {
        std::cout << std::string(8, ' ') << std::string(45, '-') << "\n" << std::endl;
        std::cout << "Timing Reports\n" << std::endl;
        std::cout << "\tLexer took " << std::to_string((lexerEnd - start).count()) << " ms\n" << std::flush;
        std::cout << "\tParser took " << std::to_string((parserEnd - lexerEnd).count()) << " ms\n" << std::flush;
        std::cout << "\tSemer took " << std::to_string((semerEnd - parserEnd).count()) << " ms\n" << std::flush;
        std::cout << "\tTotal took " << std::to_string((end - start).count()) << " ms\n" << std::flush;
    }

    return 0;
}

int version() {
    std::cout << "\n\tNouveau Compiler v" << Nouveau_VERSION_MAJOR << "." << Nouveau_VERSION_MINOR << "." << Nouveau_VERSION_PATCH << std::endl;

    return 0;
}

int help() {
    version();

    std::cout << "\n\tUsage:\n" << std::endl;
    std::cout << "\tnv [options] [filename]\n" << std::endl;

    std::cout << "\t" << padRight("nv --help", 27) << "Prints this help." << std::endl;
    std::cout << "\t" << padRight("nv --version", 27) << "Prints the version of the compiler." << std::endl;
    std::cout << "\t" << padRight("nv <filename>", 27) << "Compiles the specified file." << std::endl;
    std::cout << "\t" << padRight("nv --timings <filename>", 27) << "Compiles the specified file and prints the time it took to compile." << std::endl;

    std::cout << std::endl;

    return 0;
}

int main(int argc, char* argv[]) {
    std::map<std::string, std::string> commandLineArguments = parseCommandLineArguments(argc, argv);

    if (commandLineArguments.empty()) {
        std::cerr << RED << "\n\tError: No arguments or specified." << RESET << std::endl;
        std::cout << "\n\tRun 'nv --help' to see what you can do with the compiler !\n" << ::std::endl;

        return 1;
    }

    if (commandLineArguments.find("--version") != commandLineArguments.end()) {
        return version();
    }

    if (commandLineArguments.find("--help") != commandLineArguments.end()) {
        return help();
    }

    if (commandLineArguments.find("filename") != commandLineArguments.end()) {
        return compile(commandLineArguments);
    }
}
