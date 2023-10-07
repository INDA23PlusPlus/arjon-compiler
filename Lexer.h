//
// Created by Arvid Jonasson on 2023-10-06.
//
#pragma once
#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <stdexcept>
#include <unordered_map>
#include <ios>
#include <cctype>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <utility>
#include <istream>
#include <memory>
#include "Token.h"
#include "boost/type_index.hpp"


template<typename T>
concept InputStream = std::is_base_of_v<std::istream, T>;

template<typename T>
concept InputStreamPtr = InputStream<typename T::element_type>;

class Lexer {
    std::unique_ptr<std::istream> source_ptr;
    std::istream &source;

public:
    Lexer() = delete;

    template<InputStreamPtr T>
    explicit Lexer(T &&source) : source_ptr(std::forward<T>(source)), source(*this->source_ptr) {
        if (!this->source) {
            throw std::invalid_argument("Expected a good stream");
        }
    }

    template<InputStream T>
    explicit Lexer(T &&source) : Lexer(std::make_unique<T>(std::forward<T>(source))) {}

    [[nodiscard]] Token getNextToken();

private:
    Token parseDigit();

    Token parseAlpha();

    Token parsePunct();

    std::pair<unsigned int, unsigned int> getErrorPosition();

    struct InternalData {
        const static std::unordered_map<std::string, Keyword> keywords;
        const static std::unordered_map<std::string, Operator> operators;
        const static std::unordered_map<std::string, Punctuation> punctuations;
    };
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
