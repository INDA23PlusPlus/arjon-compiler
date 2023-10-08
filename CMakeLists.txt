cmake_minimum_required(VERSION 3.26)
project(compiler)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})

enable_testing()

add_executable(lexer_test Lexer.cpp TestLexer.cpp)

add_executable(compiler main.cpp
        Lexer.h
        Token.h
        Parser.cpp
        Parser.h
        Lexer.cpp
        ASTNode.cpp
        ASTNode.h)


add_test(NAME LexerTest COMMAND lexer_test)