//
// Created by Arvid Jonasson on 2023-10-06.
//
#pragma once
#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <stdexcept>
#include <ios>
#include <cctype>
#include <cstdio>
#include <string>
#include <utility>
#include <istream>
#include <memory>
#include <deque>
#include "Token.h"

// Used to check that input stream is derived from std::istream
template<typename T>
concept InputStream = std::is_base_of_v<std::istream, std::remove_cvref_t<T>>;

// Used to check that we can move the input stream into a unique_ptr
template<typename T>
concept InputStreamRef = InputStream<T> && std::movable<T>;

// Used to check that the argument is a unique_ptr holding an allowed input stream
template<typename T>
concept InputStreamPtr = std::is_same_v<T, std::unique_ptr<typename T::element_type>>
                         && InputStream<typename T::element_type>
                         && std::constructible_from<std::unique_ptr<std::istream>, std::add_rvalue_reference_t<T>>;

class Lexer {
public:
    using TokenAndPos = std::pair<Token, std::istream::pos_type>;
private:
    std::unique_ptr<std::istream> source;
    std::istream::pos_type lastTokenPos = 0;
    std::deque<TokenAndPos> tokens;

public:
    Lexer() = delete;

    template<InputStreamPtr T>
    explicit Lexer(T &&source) : source(std::forward<T>(source)) {
        if (!this->source) {
            throw std::invalid_argument("Expected a non null pointer");
        }
        if (!*this->source) {
            throw std::invalid_argument("Expected a good stream");
        }
    }

    template<InputStreamRef T>
    explicit Lexer(T &&source) : Lexer(std::make_unique<T>(std::forward<T>(source))) {}

    [[nodiscard]] Token getNextToken();

    [[nodiscard]] Token lookAhead(std::int_least32_t);

    std::pair<unsigned int, unsigned int> getErrorPosition();

private:
    [[nodiscard]] TokenAndPos parseNextToken();

    Token parseDigit();

    Token parseAlpha();

    Token parsePunct();
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
