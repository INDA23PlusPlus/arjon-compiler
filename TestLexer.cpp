//
// Created by Arvid Jonasson on 2023-10-07.
//
#define BOOST_TEST_MODULE LexerTest

#include <boost/test/included/unit_test.hpp>
#include "Lexer.h"
#include <sstream>

BOOST_AUTO_TEST_CASE(test_1) {
    std::stringstream ss("let a i32 = 500");
    Lexer<std::stringstream> lexer(std::move(ss));

    BOOST_CHECK(lexer.getNextToken() == Token(Keyword::Let));
    BOOST_CHECK(lexer.getNextToken() == Token(Identifier("a")));
    BOOST_CHECK(lexer.getNextToken() == Token(Keyword::i32));
    BOOST_CHECK(lexer.getNextToken() == Token(Operator::Assignment));
    BOOST_CHECK(lexer.getNextToken() == Token(IntegerLiteral(500)));
    BOOST_CHECK(lexer.getNextToken() == Token(EndToken()));
}

BOOST_AUTO_TEST_CASE(test_2) {
    std::stringstream ss("if (a == 500.0) {}");
    Lexer<std::stringstream> lexer(std::move(ss));

    BOOST_CHECK(lexer.getNextToken() == Token(Keyword::If));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::OpenParen));
    BOOST_CHECK(lexer.getNextToken() == Token(Identifier("a")));
    BOOST_CHECK(lexer.getNextToken() == Token(Operator::Equal));
    BOOST_CHECK(lexer.getNextToken() == Token(FloatLiteral(500.0)));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::CloseParen));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::OpenBrace));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::CloseBrace));
    BOOST_CHECK(lexer.getNextToken() == Token(EndToken()));
}