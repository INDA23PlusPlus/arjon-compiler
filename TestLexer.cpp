//
// Created by Arvid Jonasson on 2023-10-07.
//
#define BOOST_TEST_MODULE LexerTest

#include <boost/test/included/unit_test.hpp>
#include "Lexer.h"
#include <sstream>

BOOST_AUTO_TEST_CASE(test_1) {
    std::istringstream ss("Let a = 500;");
    auto ptr = std::make_unique<std::istringstream>(std::move(ss));
    Lexer lexer(std::move(ptr));

    BOOST_CHECK(lexer.getNextToken() == Token(Keyword::Let));
    BOOST_CHECK(lexer.getNextToken() == Token(Identifier("a")));
    BOOST_CHECK(lexer.getNextToken() == Token(Operator::Assignment));
    BOOST_CHECK(lexer.getNextToken() == Token(IntegerLiteral(500)));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::Semicolon));
    BOOST_CHECK(lexer.getNextToken() == Token(EndToken()));
}

BOOST_AUTO_TEST_CASE(test_2) {
    std::istringstream ss("if (a == 500) return 10;");
    Lexer lexer(std::move(ss));

    BOOST_CHECK(lexer.getNextToken() == Token(Keyword::If));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::OpenParen));
    BOOST_CHECK(lexer.getNextToken() == Token(Identifier("a")));
    BOOST_CHECK(lexer.getNextToken() == Token(Operator::Equal));
    BOOST_CHECK(lexer.getNextToken() == Token(IntegerLiteral(500)));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::CloseParen));
    BOOST_CHECK(lexer.getNextToken() == Token(Keyword::Return));
    BOOST_CHECK(lexer.getNextToken() == Token(IntegerLiteral(10)));
    BOOST_CHECK(lexer.getNextToken() == Token(Punctuation::Semicolon));
    BOOST_CHECK(lexer.getNextToken() == Token(EndToken()));
}

BOOST_AUTO_TEST_CASE(test_3) {
    BOOST_CHECK_THROW(Lexer(std::unique_ptr<std::istream>{}), std::invalid_argument);
    Lexer lex(std::ifstream{});
}