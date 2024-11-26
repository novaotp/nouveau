cmake_minimum_required(VERSION 3.14)

project(nouveau VERSION 0.0.8 DESCRIPTION "A statically-typed general-purpose compiled programming language." LANGUAGES CXX)
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

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(nouveau_tests
        tests/test_ast.cpp
        tests/test_lexer.cpp
        tests/test_parser.cpp
        tests/test_semer.cpp
        tests/test_utils.cpp
        tests/test_token.cpp
        source/ast.cpp
        source/lexer.cpp
        source/parser.cpp
        source/semer.cpp
        source/token.cpp
        source/utils.cpp
    )

    target_link_libraries(nouveau_tests PRIVATE Catch2::Catch2WithMain)

    target_include_directories(nouveau_tests PRIVATE source/headers)
    target_include_directories(nouveau_tests PRIVATE "${PROJECT_BINARY_DIR}")

    target_compile_features(nouveau_tests PRIVATE cxx_std_17)

    include(CTest)
    include(Catch)
    catch_discover_tests(nouveau_tests)
endif()
