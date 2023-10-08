//
// Created by Arvid Jonasson on 2023-10-06.
//
#pragma once
#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Lexer.h"
#include "ASTNode.h"
#include <utility>
#include <fstream>
#include <vector>

class Parser {
    Lexer lexer;
    Token currentToken;
    std::vector<AST::NodePtr> statements;
public:
    Parser() = delete;

    template<typename T>
    explicit Parser(T &&source): lexer(std::forward<T>(source)) {}

    Parser &parse_program();

private:
    AST::FunctionNodePtr parse_function();

    AST::FunctionDeclarationNodePtr parse_function_declaration();

    AST::VariableDeclarationNodePtr parse_variable_declaration();

    AST::NodePtr parse_expression();

    AST::FunctionType parse_function_type();

    AST::BlockNodePtr parse_block();

    AST::ExpressionBlockNodePtr parse_expression_block();

    AST::NodePtr parse_statement();

    template<typename T>
    void throw_syntax_error(T &&error_message) {
        auto [line, position] = lexer.getErrorPosition();
        throw SyntaxErrorException(std::forward<T>(error_message), line, position);
    }

    template<typename T, typename U>
    T &check_expected_or_throw(U &&error_message) {
        if(!std::holds_alternative<T>(currentToken)) {
            throw_syntax_error(error_message);
        }
        return std::get<T>(currentToken);
    }

    template<typename T, typename U>
    T &get_expected_or_throw(U &&error_message) {
        currentToken = lexer.getNextToken();
        return check_expected_or_throw<T>(std::forward<U>(error_message));
    }

    template<typename T, typename U>
    auto &expect_next_token(const T &expectedToken, U &&error_message) {
        auto token = get_expected_or_throw<T>(std::forward<U>(error_message));
        if(token != expectedToken)
            throw_syntax_error(std::forward<U>(error_message));
        return *this;
    }

    template<typename T, typename U>
    auto &expect_current_token(const T &expectedToken, U &&error_message) {
        auto token = check_expected_or_throw<T>(std::forward<U>(error_message));
        if(token != expectedToken)
            throw_syntax_error(std::forward<U>(error_message));
        return *this;
    }

    template<typename T>
    bool is_current_token(const T &token) {
        return std::holds_alternative<T>(currentToken) && std::get<T>(currentToken) == token;
    }

    __attribute__((always_inline))
    Token &consume_token() {
        return currentToken = lexer.getNextToken();
    }
};


#endif //COMPILER_PARSER_H
