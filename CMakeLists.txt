cmake_minimum_required(VERSION 3.14)

project(nouveau VERSION 0.0.5 DESCRIPTION "A statically-typed general-purpose compiled programming language." LANGUAGES CXX)
cmake_policy(SET CMP0048 NEW)

add_executable(nouveau
    source/ast.cpp
    source/lexer.cpp
    source/parser.cpp
    source/semer.cpp
    source/token.cpp
    source/utils.cpp
    source/main.cpp
)

configure_file(source/config.h.in "${CMAKE_BINARY_DIR}/config.h" @ONLY)

target_include_directories(nouveau PRIVATE source/headers)
target_include_directories(nouveau PRIVATE "${PROJECT_BINARY_DIR}")

target_compile_features(nouveau PRIVATE cxx_std_17)
target_compile_options(nouveau PRIVATE -Wall)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
