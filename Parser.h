//
// Created by Arvid Jonasson on 2023-10-06.
//
#pragma once
#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Lexer.h"
#include <utility>
#include <fstream>

class Parser {
    Lexer<std::ifstream> lexer;

public:
    Parser() = delete;

    template<typename T>
    explicit Parser(T &&source) : lexer(std::forward<T>(source)) {}
};


#endif //COMPILER_PARSER_H
