#include "codegen.hpp"
#include <fstream>
#include <string>
#include <iostream>

Codegen::Codegen(Program program) : program(program) {};
Codegen::~Codegen() {};

std::string Codegen::writeAssembly() {
    Value assembly({}, {}, {});

    for (size_t i = 0; i < this->program.body.size(); i++) {
        const auto& node = this->program.body[i];

        Value val = std::visit([](const auto& ptr) -> Value {
            using Type = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<Type, Expression>) {
                return std::visit([](const auto& expr) -> Value {
                    /* using SType = std::decay_t<decltype(expr)>;

                    std::cout << typeid(SType).name() << std::endl; */

                    return expr.codegen();
                }, *ptr);
            } else if constexpr (std::is_same_v<Type, Statement>) {
                return std::visit([](const auto& expr) -> Value {
                    return expr.codegen();
                }, *ptr);
            } else {
                throw std::runtime_error("Unknown Node Type in code generation");
            }
        }, node);

        assembly.data.insert(assembly.data.end(), val.data.begin(), val.data.end());
        assembly.bss.insert(assembly.bss.end(), val.bss.begin(), val.bss.end());
        assembly.instructions.insert(assembly.instructions.end(), val.instructions.begin(), val.instructions.end());
    }

    std::string assemblyString;

    assemblyString += "section .data\n";
    for (size_t i = 0; i < assembly.data.size(); i++) {
        assemblyString += std::string("\t") + assembly.data.at(i) + std::string("\n");
    }

    assemblyString += "\nsection .bss";
    for (size_t i = 0; i < assembly.bss.size(); i++) {
        assemblyString += std::string("\t") + assembly.bss.at(i) + std::string("\n");
    }

    assemblyString += "\n\nsection .text\n\tglobal _start\n\n_start:\n";
    for (size_t i = 0; i < assembly.instructions.size(); i++) {
        assemblyString += std::string("\t") + assembly.instructions.at(i) + std::string("\n");
    }

    assemblyString += "\t; Exit the program\n\tmov rax, 60\n\txor rdi, rdi\n\tsyscall\n";

    return assemblyString;
}

void Codegen::generate() {
    std::string assembly = this->writeAssembly();

    std::ofstream out("output/out.asm");
    out << assembly;
    out.close();
}
