//
// Created by Arvid Jonasson on 2023-10-06.
//

#include "Lexer.h"
#include <cctype>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <utility>

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
        {"{", Punctuation::CloseBrace},
};

static Token parseDigit(std::ifstream &source) {
    std::string lexeme;
    bool is_float = false;

    do {
        if (source.peek() == '.') {
            if (is_float) {
                // Undo changes in stream position to deliver an error message pointing to the start of the syntax error
                source.seekg(-static_cast<std::ifstream::pos_type>(lexeme.length()), std::ios::cur);
                throw SyntaxErrorException("Invalid floating point literal");
            }
            is_float = true;
        }
        lexeme.push_back(static_cast<std::string::value_type>(source.get()));
    } while (std::isdigit(source.peek()) || source.peek() == '.');

    if (is_float) { return FloatLiteral(std::stold(lexeme)); }
    return IntegerLiteral(std::stoull(lexeme));
}

static Token parseAlpha(std::ifstream &source) {
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

static Token parsePunct(std::ifstream &source) {
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

static std::pair<unsigned int, unsigned int> getErrorPosition(std::ifstream &source) {
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

Token Lexer::getNextToken() {
    while (std::isspace(source.peek())) {
        source.ignore();
    }

    auto c = source.peek();

    if (c == EOF) {
        return EndToken();
    }
    try {
        if (std::isdigit(c)) {
            return parseDigit(source);
        } else if (std::isalpha(c)) {
            return parseAlpha(source);
        } else if (std::ispunct(c)) {
            return parsePunct(source);
        }
    } catch (SyntaxErrorException const &e) {
        auto [line, position] = getErrorPosition(source);
        throw SyntaxErrorException(e.what(), line, position);
    }

    throw std::runtime_error("Unknown character with value " + std::to_string(c));
}