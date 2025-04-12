#include "codegen.hpp"
#include <fstream>
#include <string>
#include <iostream>

Codegen::Codegen(Program program) : program(program) {};
Codegen::~Codegen() {};

std::string Codegen::writeAssembly() {
    std::string assembly = "";

    for (size_t i = 0; i < this->program.body.size(); i++) {
        const auto& node = this->program.body[i];

        assembly += std::visit([](const auto& ptr) -> std::string {
            using Type = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<Type, Expression>) {
                return std::visit([](const auto& expr) -> std::string {
                    using SType = std::decay_t<decltype(expr)>;

                    std::cout << typeid(SType).name() << std::endl;

                    return expr.codegen();
                }, *ptr);
            }/*  else if constexpr (std::is_same_v<Type, Statement>) {
                std::visit([](const auto& expr) {
                    printStatement(expr, 0);
                }, *ptr);
            } */ else {
                std::cout << "Unknown Node Type in code generation" << std::endl;
                return "";
            }
        }, node);
    }

    return assembly;
}

void Codegen::generate() {
    std::string assembly = this->writeAssembly();

    std::ofstream out("output/out.asm");
    out << assembly;
    out.close();
}
