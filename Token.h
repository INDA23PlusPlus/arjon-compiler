//
// Created by Arvid Jonasson on 2023-10-06.
//
#pragma once
#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <string>
#include <variant>

using EndToken = std::monostate;
using IntegerLiteral = unsigned long long int;
using FloatLiteral = long double;
using Identifier = std::string;

enum class Punctuation {
    // Parentheses
    OpenParen,      // '('
    CloseParen,     // ')'

    // Braces
    OpenBrace,      // '{'
    CloseBrace,     // '}'

    // End of statement
    NewLine,        // '\n'

    Colon,          // ':'
    Comma,          // ','

    /* Might implement in the future
    // Brackets
    OpenBracket,    // '['
    CloseBracket,   // ']'

    Dot,            // '.'
    Semicolon,      // ';'
    DoubleColon,    // '::'
    Ellipsis,       // '...'
    */
};

enum class Operator {
    // Arithmetic operators
    Add,            // '+'
    Subtract,       // '-'
    Multiply,       // '*'
    Divide,         // '/'
    Modulus,        // '%'


    // Assignment operators
    Assignment,     // '='
    /* Saving for later
    AddAssign,      // '+='
    SubAssign,      // '-='
    MulAssign,      // '*='
    DivAssign,      // '/='
    ModAssign,      // '%='
    */

    // Relational operators
    Equal,          // '=='
    NotEqual,       // '!='
    LessThan,       // '<'
    GreaterThan,    // '>'
    LessThanOrEq,   // '<='
    GreaterThanOrEq,// '>='

    // Logical operators
    LogicalAnd,     // '&&'
    LogicalOr,      // '||'
    LogicalNot,     // '!'

    // Other operators
    RightArrow,     // '->'
    LeftArrow,      // '<-'

    /* Might implement in the future
    // Bitwise operators
    BitwiseAnd,     // '&'
    BitwiseOr,      // '|'
    BitwiseXor,     // '^'
    BitwiseNot,     // '~'
    LeftShift,      // '<<'
    RightShift,     // '>>'

    // Other
    Ternary,        // '?'
    Colon,          // ':'
    Increment,      // '++'
    Decrement,      // '--'
    */
};

enum class Keyword {
    Return,
    While,
    If,
    Fn,
};

enum class Type {
    i32,
    i64,
    u32,
    u64,
    f32,
    f64,
};

using Token = std::variant<
        EndToken,           // Signals end of tokens (default)
        Identifier,         // For identifiers
        IntegerLiteral,     // For integer literals
        FloatLiteral,       // For float literals
        Operator,           // For operators
        Punctuation,        // For punctuation (see enum for reference)
        Keyword,            // For keywords
        Type                // For types
>;

#endif //COMPILER_TOKEN_H
