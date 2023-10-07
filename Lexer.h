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
#include "Token.h"
#include "boost/type_index.hpp"

template<typename SourceType>
class Lexer {
    SourceType source;

public:
    Lexer() = delete;

    template<typename T>
    explicit Lexer(T &&source) : source(std::forward<T>(source)) {
        if (!this->source) {
            if (std::is_same<T, SourceType>())
                throw std::invalid_argument(std::string("Expected an open ") +
                                            boost::typeindex::type_id_with_cvr<SourceType>().pretty_name());
            else
                throw std::runtime_error("Couldn't open source file");
        }
    }

    [[nodiscard]] Token getNextToken();

private:
    Token parseDigit();

    Token parseAlpha();

    Token parsePunct();

    std::pair<unsigned int, unsigned int> getErrorPosition();
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

template<typename SourceType>
Token Lexer<SourceType>::getNextToken() {
    while (std::isspace(source.peek())) {
        source.ignore();
    }

    auto c = source.peek();

    if (c == EOF) {
        return EndToken();
    }
    try {
        if (std::isdigit(c)) {
            return parseDigit();
        } else if (std::isalpha(c)) {
            return parseAlpha();
        } else if (std::ispunct(c)) {
            return parsePunct();
        }
    } catch (SyntaxErrorException const &e) {
        auto [line, position] = getErrorPosition();
        throw SyntaxErrorException(e.what(), line, position);
    }

    throw std::runtime_error("Unknown character with value " + std::to_string(c));
}

static std::unordered_map<std::string, Keyword> keywords{
        {"return", Keyword::Return},
        {"fn",     Keyword::Fn},
        {"while",  Keyword::While},
        {"if",     Keyword::If},
        {"let",    Keyword::Let},
        {"main",   Keyword::Main},
        {"i32",    Keyword::i32},
        {"i64",    Keyword::i64},
        {"u32",    Keyword::u32},
        {"u64",    Keyword::u64},
        {"f32",    Keyword::f32},
        {"f64",    Keyword::f64},
};

static std::unordered_map<std::string, Operator> operators{
        // Arithmetic operators
        {"+",  Operator::Add},
        {"-",  Operator::Subtract},
        {"*",  Operator::Multiply},
        {"/",  Operator::Divide},
        {"%",  Operator::Modulus},

        // Assignment operator
        {"=",  Operator::Assignment},

        // Relational operators
        {"==", Operator::Equal},
        {"!=", Operator::NotEqual},
        {"<",  Operator::LessThan},
        {">",  Operator::GreaterThan},
        {"<=", Operator::LessThanOrEq},
        {">=", Operator::GreaterThanOrEq},

        // Logical operators
        {"&&", Operator::LogicalAnd},
        {"||", Operator::LogicalOr},
        {"!",  Operator::LogicalNot},
};

static std::unordered_map<std::string, Punctuation> punctuations{
        {"(", Punctuation::OpenParen},
        {")", Punctuation::CloseParen},
        {"{", Punctuation::OpenBrace},
        {"}", Punctuation::CloseBrace},
};

template<typename SourceType>
Token Lexer<SourceType>::parseDigit() {
    std::string lexeme;
    bool is_float = false;

    do {
        if (source.peek() == '.') {
            if (is_float) {
                // Undo changes in stream position to deliver an error message pointing to the start of the syntax error
                source.seekg(-static_cast<SourceType::pos_type>(lexeme.length()), std::ios::cur);
                throw SyntaxErrorException("Invalid floating point literal");
            }
            is_float = true;
        }
        lexeme.push_back(static_cast<std::string::value_type>(source.get()));
    } while (std::isdigit(source.peek()) || source.peek() == '.');

    if (is_float) { return FloatLiteral(std::stold(lexeme)); }
    return IntegerLiteral(std::stoull(lexeme));
}

template<typename SourceType>
Token Lexer<SourceType>::parseAlpha() {
    std::string lexeme;

    do {
        lexeme.push_back(static_cast<std::string::value_type>(source.get()));
    } while (std::isalnum(source.peek()));

    auto it = keywords.find(lexeme);

    if (it != keywords.end()) {
        return it->second;
    }

    return Identifier(std::move(lexeme));
}

template<typename SourceType>
Token Lexer<SourceType>::parsePunct() {
    std::string lexeme;

    do {
        lexeme.push_back(static_cast<std::string::value_type>(source.get()));
    } while (std::ispunct(source.peek()));

    do {
        const auto oper_iter = operators.find(lexeme);
        if (oper_iter != std::cend(operators)) {
            return oper_iter->second;
        }
        const auto punc_iter = punctuations.find(lexeme);
        if (punc_iter != std::cend(punctuations)) {
            return punc_iter->second;
        }
        lexeme.pop_back();
        source.unget();
    } while (!lexeme.empty());

    throw SyntaxErrorException("Unexpected operator");
}

template<typename SourceType>
std::pair<unsigned int, unsigned int> Lexer<SourceType>::getErrorPosition() {
    unsigned int line = 1, position = 1;

    auto error_pos = source.tellg();
    source.seekg(0, std::ios::beg);

    while (source.tellg() < error_pos) {
        switch (source.get()) {
            case '\n':
                ++line;
                position = 1;
                break;
            default:
                ++position;
                break;
        }
    }
    return std::make_pair(line, position);
}

#endif //COMPILER_LEXER_H
