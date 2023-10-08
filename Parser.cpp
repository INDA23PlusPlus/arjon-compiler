//
// Created by Arvid Jonasson on 2023-10-06.
//

#include "Parser.h"

Parser &Parser::parse_program() {
    while(!holds_alternative<EndToken>(currentToken = lexer.getNextToken())) {
        statements.emplace_back(std::move(parse_function()));
    }
    return *this;
}

AST::FunctionDeclarationNodePtr Parser::parse_function_declaration() {
    // X: (i32, f32) -> i32 = fn(a, b) { ... }
    // Might add support to automatically deduce all types in the future, like:
    // X: = fn(a: i32, b: f32) -> i32 { ... }
    Identifier name = std::move(check_expected_or_throw<Identifier>("Expected function name"));

    expect_next_token(Punctuation::Colon, "Expected colon separating identifier from type");

    currentToken = lexer.getNextToken();

    auto function_type = parse_function_type();

    expect_current_token(Operator::Equal, "Expected assignment operator after function declaration");

    currentToken = lexer.getNextToken();

    auto function = parse_function();

    return std::make_unique<AST::FunctionDeclarationNode>
            (std::move(name), std::move(function_type), std::move(function));
}

AST::FunctionType Parser::parse_function_type() {
    std::vector<Type> parameterTypes;
    std::optional<Type> returnType;

    expect_current_token(Punctuation::OpenParen, "Expected opening parentheses for function declaration");

    while(std::holds_alternative<Type>(currentToken = lexer.getNextToken())) {
        parameterTypes.emplace_back(std::get<Type>(currentToken));
        auto error_msg = "Expected closing parenthesis or comma after function type";
        auto commaOrClosingParen = get_expected_or_throw<Punctuation>(error_msg);
        if(commaOrClosingParen == Punctuation::CloseParen)
            break;
        if(commaOrClosingParen != Punctuation::Comma)
            throw_syntax_error(error_msg);
    }

    // Here we either have an assignment operator or a '->' followed by a return type
    expect_current_token(Punctuation::CloseParen, "Expected closing parenthesis after function types");
    {
        auto error_msg = "Expected either assignment operator or a '->' operator followed by the return type";
        auto token = get_expected_or_throw<Operator>(error_msg);
        if(token != Operator::Equal && token != Operator::RightArrow)
            throw_syntax_error(error_msg);
        if(token == Operator::RightArrow) {
            returnType = get_expected_or_throw<Type>("Expected a return type");
            consume_token();
        }
    }

    return {std::move(parameterTypes), returnType};
}

AST::FunctionNodePtr Parser::parse_function() {
    // fn(a: i32, b: f32) -> i32 { ... }
    // or
    // fn(a, b) { ... }

    expect_current_token(Keyword::Fn, "Expected the fn keyword to declare the function");
    expect_next_token(Punctuation::OpenParen, "Expected opening parenthesis after the fn keyword");

    AST::ParameterList parameterList;
    currentToken = lexer.getNextToken();
    while(std::holds_alternative<Identifier>(currentToken)) {
        auto name = std::move(std::get<Identifier>(currentToken));
        std::optional<Type> type;

        auto error_msg = "Expected comma, colon or closing parenthesis after variable declaration";
        auto punctToken = get_expected_or_throw<Punctuation>(error_msg);


        if(punctToken == Punctuation::Comma) {
            type = get_expected_or_throw<Type>("Expected type");
            punctToken = get_expected_or_throw<Punctuation>(error_msg);
        }
        if(punctToken == Punctuation::Colon) {
            currentToken = lexer.getNextToken();
        }
        parameterList.emplace_back(std::move(name), type);
    }

    expect_next_token(Punctuation::CloseParen, "Expected closing parenthesis");

    std::optional<Type> returnType;

    consume_token();

    if(is_current_token(Operator::RightArrow)) {
        returnType = get_expected_or_throw<Type>("Expected a return type");
        consume_token();
    }

    AST::BlockNodePtr functionBody = parse_block();
    return std::make_unique<AST::FunctionNode>(std::move(parameterList), returnType, std::move(functionBody));
}

AST::ExpressionBlockNodePtr Parser::parse_expression_block() {
    AST::ExpressionBlockNode expressionBlockNode;
    expressionBlockNode.block = parse_block();
    return std::make_unique<AST::ExpressionBlockNode>(std::move(expressionBlockNode));
}

AST::BlockNodePtr Parser::parse_block() {
    expect_current_token(Punctuation::OpenBrace, "Expected opening brace before block.");
    consume_token();

    AST::BlockNode block;

    while (!is_current_token(Punctuation::CloseBrace)) {
        block.addStatement(parse_statement());
    }
    consume_token();
    return std::make_unique<AST::BlockNode>(std::move(block));
}

AST::NodePtr Parser::parse_statement() {
    // Figure out what the statement is through forward looking method
    // Statement can be function declaration, variable declaration, if statement, while loop, expression

    if(is_current_token(Keyword::If)) {
        // Parse if
    } else if(is_current_token(Keyword::While)) {
        // Parse while
    } else if(std::holds_alternative<Identifier>(currentToken) && std::holds_alternative<Punctuation>(lexer.lookAhead(1))
            && std::get<Punctuation>(lexer.lookAhead(1)) == Punctuation::Colon) {
        // Declaraion
        if(std::holds_alternative<Type>(lexer.lookAhead(2))) {
            // Variable declaration
            return parse_variable_declaration();
        } else {
            // Function declaration
            return parse_function_declaration();
        }
    } else {
        return parse_expression();
    }
    std::unreachable();
}

AST::VariableDeclarationNodePtr Parser::parse_variable_declaration() {
    auto name = std::move(check_expected_or_throw<Identifier>("Expected variable name for variable declaration"));
    expect_next_token(Punctuation::Colon, "Expected a colon before variable type");
    auto type = get_expected_or_throw<Type>("Expected a variable type");
    expect_next_token(Operator::Assignment, "Expected an assignment operator after variable declaration");

    consume_token();
    AST::NodePtr block;
    if(is_current_token(Punctuation::OpenBrace)) {
        consume_token();
        block = parse_expression_block();
        expect_current_token(Punctuation::CloseBrace, "Expected a closing brace after expression block");
        consume_token();
    } else {
        block = parse_expression();
    }
    return std::make_unique<AST::VariableDeclarationNode>(AST::Variable {std::move(name), type}, std::move(block));
}

AST::NodePtr Parser::parse_expression() {
    // This should basically function calls, variable references and binary operations
}
