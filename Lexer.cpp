//
// Created by Arvid Jonasson on 2023-10-07.
//
#include "Lexer.h"
#include <unordered_map>

struct InternalData {
    const static std::unordered_map<std::string, Keyword> keywords;
    const static std::unordered_map<std::string, Operator> operators;
    const static std::unordered_map<std::string, Punctuation> punctuations;
};

const std::unordered_map<std::string, Keyword> InternalData::keywords{
        {"return",   Keyword::Return},
        {"fn",       Keyword::Fn},
        {"if",       Keyword::If},
        {"else",     Keyword::Else},
        {"let",      Keyword::Let},
};

const std::unordered_map<std::string, Operator> InternalData::operators{
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

const std::unordered_map<std::string, Punctuation> InternalData::punctuations{
        {"(",  Punctuation::OpenParen},
        {")",  Punctuation::CloseParen},
        {"{",  Punctuation::OpenBrace},
        {"}",  Punctuation::CloseBrace},
        {",",  Punctuation::Comma},
        {";",  Punctuation::Semicolon},
};

Token Lexer::getNextToken() {
    TokenAndPos token;
    if (!tokens.empty()) {
        token = std::move(tokens.front());
        tokens.pop_front();
    } else {
        token = parseNextToken();
    }
    lastTokenPos = token.second;
    return token.first;
}

Lexer::TokenAndPos Lexer::parseNextToken() {
    while (std::isspace(source->peek())) {
        source->ignore();
    }

    auto c = source->peek();
    auto tokenPos = source->tellg() + static_cast<std::istream::pos_type>(1);

    if (c == EOF) {
        return std::make_pair(EndToken(), tokenPos);
    }
    if (c == '/') {
        source->ignore();

        // if comment
        if (source->peek() == '/') {
            source->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return parseNextToken();
        }
        source->unget();
    }
    try {
        if (std::isdigit(c)) {
            return std::make_pair(parseDigit(), tokenPos);
        } else if (std::isalpha(c)) {
            return std::make_pair(parseAlpha(), tokenPos);
        } else if (std::ispunct(c)) {
            return std::make_pair(parsePunct(), tokenPos);
        }
    } catch (SyntaxErrorException const &e) {
        auto [line, position] = getErrorPosition();
        throw SyntaxErrorException(e.what(), line, position);
    }

    throw std::runtime_error("Unknown character with value " + std::to_string(c));
}

Token Lexer::lookAhead(std::int_least32_t x) {
    if (x <= 0)
        throw std::invalid_argument("Expected to look ahead more than 0 tokens, you asked to look ahead "
                                    + std::to_string(x) + " elements");
    while (x <= tokens.size()) {
        tokens.emplace_back(std::move(parseNextToken()));
    }
    return tokens[x - 1].first;
}

Token Lexer::parseDigit() {
    std::string lexeme;

    do {
        lexeme.push_back(static_cast<std::string::value_type>(source->get()));
    } while (std::isdigit(source->peek()));

    return IntegerLiteral(std::stoull(lexeme));
}

Token Lexer::parseAlpha() {
    std::string lexeme;

    do {
        lexeme.push_back(static_cast<std::string::value_type>(source->get()));
    } while (std::isalnum(source->peek()));

    const auto keyword_it = InternalData::keywords.find(lexeme);
    if (keyword_it != std::cend(InternalData::keywords)) {
        return keyword_it->second;
    }

    return Identifier(std::move(lexeme));
}

Token Lexer::parsePunct() {
    std::string lexeme;

    do {
        lexeme.push_back(static_cast<std::string::value_type>(source->get()));
    } while (std::ispunct(source->peek()));

    do {
        const auto oper_iter = InternalData::operators.find(lexeme);
        if (oper_iter != std::cend(InternalData::operators)) {
            return oper_iter->second;
        }
        const auto punc_iter = InternalData::punctuations.find(lexeme);
        if (punc_iter != std::cend(InternalData::punctuations)) {
            return punc_iter->second;
        }
        source->unget();
        lexeme.pop_back();
    } while (!lexeme.empty());

    throw SyntaxErrorException("Unexpected operator");
}

std::pair<unsigned int, unsigned int> Lexer::getErrorPosition() {
    unsigned int line = 1, position = 1;

    auto error_pos = lastTokenPos;

    // Restart from the beginning and count the number of lines and where in the line we are
    source->seekg(0, std::ios::beg);
    while (source->tellg() < error_pos) {
        switch (source->get()) {
            case '\n':
                ++line;
                position = 1;
                break;
            default:
                ++position;
                break;
        }
    }
    // Return the error position
    return std::make_pair(line, position);
}