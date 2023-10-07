//
// Created by Arvid Jonasson on 2023-10-07.
//
#include "Lexer.h"


const std::unordered_map<std::string, Keyword> InternalData::keywords{
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
        {"(", Punctuation::OpenParen},
        {")", Punctuation::CloseParen},
        {"{", Punctuation::OpenBrace},
        {"}", Punctuation::CloseBrace},
};