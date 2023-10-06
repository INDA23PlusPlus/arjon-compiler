//
// Created by Arvid Jonasson on 2023-10-06.
//
#pragma once
#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <fstream>
#include <stdexcept>
#include "Token.h"

class Lexer {
    std::ifstream source;

public:
    Lexer() = delete;

    template<typename T>
    explicit Lexer(T &&source) : source(std::forward<T>(source)) {
        if (!this->source.is_open()) {
            if (std::is_same<T, std::ifstream>())
                throw std::invalid_argument("Expected an open ifstream");
            else
                throw std::runtime_error("Couldn't open source file");
        }
    }

    [[nodiscard]] Token getNextToken();
};

class SyntaxErrorException : public std::runtime_error {
private:
    unsigned int line;
    unsigned int position;

public:
    SyntaxErrorException() = delete;

    explicit SyntaxErrorException(const std::string &message, unsigned int line = 0, unsigned int position = 0)
            : std::runtime_error(message), line(line), position(position) {}

    [[nodiscard]] unsigned int getLine() const { return line; }

    [[nodiscard]] unsigned int getPosition() const { return position; }
};

#endif //COMPILER_LEXER_H
