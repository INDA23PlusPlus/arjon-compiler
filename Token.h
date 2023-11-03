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
using Identifier = std::string;

enum class Punctuation {
    // Parentheses
    OpenParen,      // '('
    CloseParen,     // ')'

    // Braces
    OpenBrace,      // '{'
    CloseBrace,     // '}'

    // End of statement
    Semicolon,      // ';'
    Comma,          // ','

    /* Might implement in the future
    // End of statement
    NewLine,        // '\n'

    Colon,          // ':'

    // Brackets
    OpenBracket,    // '['
    CloseBracket,   // ']'

    Dot,            // '.'
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

    /* Might implement in the future
    // Other operators
    RightArrow,     // '->'
    LeftArrow,      // '<-'

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
    If,
    Else,
    Fn,
    Let,
};

using Token = std::variant<
        EndToken,           // Signals end of tokens (default)
        Identifier,         // For identifiers
        IntegerLiteral,     // For integer literals
        Operator,           // For operators
        Punctuation,        // For punctuation (see enum for reference)
        Keyword             // For keywords
>;

#endif //COMPILER_TOKEN_H
