//
// Created by Arvid Jonasson on 2023-10-06.
//

#include "Parser.h"

Parser &Parser::parse_program() {
    while (!holds_alternative<EndToken>(currentToken = lexer.getNextToken())) {
        statements.emplace_back(std::move(parse_statement()));
    }
    return *this;
}


std::pair<std::vector<Type>, std::optional<Type>> Parser::parse_declaration_type() {
    std::vector<Type> parameterTypes;
    if (is_current_token(Punctuation::OpenParen)) {
        while (std::holds_alternative<Type>(currentToken = lexer.getNextToken())) {
            parameterTypes.emplace_back(std::get<Type>(currentToken));
            auto error_msg = "Expected closing parenthesis or comma after function type";
            auto commaOrClosingParen = get_expected_or_throw<Punctuation>(error_msg);
            if (commaOrClosingParen == Punctuation::CloseParen)
                break;
            if (commaOrClosingParen != Punctuation::Comma)
                throw_syntax_error(error_msg);
        }
        expect_current_token(Punctuation::CloseParen, "Expected closing parenthesis after function types");
        consume_token();
        if (is_current_token(Operator::RightArrow))
            consume_token();
    }

    std::optional<Type> returnType;
    if (std::holds_alternative<Type>(currentToken)) {
        returnType = std::get<Type>(currentToken);
        consume_token();
    }

    return std::make_pair(std::move(parameterTypes), returnType);
}

AST::DeclarationNodePtr Parser::parse_declaration() {
    Identifier name = std::move(check_expected_or_throw<Identifier>("Expected function name"));

    expect_next_token(Punctuation::Colon, "Expected colon separating identifier from type");

    currentToken = lexer.getNextToken();

    auto declaration_type = parse_declaration_type();

    expect_current_token(Operator::Assignment, "Expected assignment operator after function declaration");

    currentToken = lexer.getNextToken();

    auto expression = parse_expression();

    return std::make_unique<AST::DeclarationNode>
            (std::move(name), std::move(declaration_type.first), std::move(declaration_type.second),
             std::move(expression));
}

AST::FunctionNodePtr Parser::parse_function() {
    expect_current_token(Keyword::Fn, "Expected the fn keyword to declare the function");
    consume_token();


    auto parameterList = parse_parameter_list();

    expect_current_token(Punctuation::CloseParen, "Expected closing parenthesis");

    consume_token();
    std::optional<Type> returnType;
    if (is_current_token(Operator::RightArrow)) {
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
    while (is_current_token(Punctuation::NewLine))
        consume_token();
    if (is_current_token(EndToken()))
        return {};
    // Figure out what the statement is through forward-looking method
    // Statement can be function declaration, variable declaration, if statement, while loop, expression

    if (is_current_token(Keyword::If)) {
        // Parse if
    } else if (is_current_token(Keyword::While)) {
        // Parse while
    } else if (std::holds_alternative<Identifier>(currentToken)
               && std::holds_alternative<Punctuation>(lexer.lookAhead(1))
               && std::get<Punctuation>(lexer.lookAhead(1)) == Punctuation::Colon) {
        // Declaration
        return parse_declaration();
    } else {
        return parse_expression();
    }
    std::unreachable();
}

AST::ParameterList Parser::parse_parameter_list() {
    AST::ParameterList parameterList;

    expect_current_token(Punctuation::OpenParen, "Expected opening parenthesis");
    currentToken = lexer.getNextToken();
    while (std::holds_alternative<Identifier>(currentToken)) {
        auto name = std::move(std::get<Identifier>(currentToken));
        std::optional<Type> type;

        auto error_msg = "Expected comma, colon or closing parenthesis after variable declaration";
        auto punctToken = get_expected_or_throw<Punctuation>(error_msg);


        if (punctToken == Punctuation::Comma) {
            type = get_expected_or_throw<Type>("Expected type");
            punctToken = get_expected_or_throw<Punctuation>(error_msg);
        }
        if (punctToken == Punctuation::Colon) {
            currentToken = lexer.getNextToken();
        }
        parameterList.emplace_back(std::move(name), type);
    }
    consume_token();
    return parameterList;
}

AST::NodePtr Parser::parse_expression() {
    return parse_addition_subtraction();
}

// Highest precedence: multiplication and division
AST::NodePtr Parser::parse_multiplication_division() {
    auto left = parse_function_call_or_literal();

    while (is_current_token(Operator::Multiply) || is_current_token(Operator::Divide)) {
        auto op = std::get<Operator>(currentToken);
        consume_token();
        auto right = parse_function_call_or_literal();
        left = std::make_unique<AST::BinaryOpNode>(op, std::move(left), std::move(right));
    }

    return left;
}

// Next precedence: addition and subtraction
AST::NodePtr Parser::parse_addition_subtraction() {
    auto left = parse_multiplication_division();

    while (is_current_token(Operator::Add) || is_current_token(Operator::Subtract)) {
        auto op = std::get<Operator>(currentToken);
        consume_token();
        auto right = parse_multiplication_division();
        left = std::make_unique<AST::BinaryOpNode>(op, std::move(left), std::move(right));
    }

    return left;
}

// Handle function calls, literals, and variable references
AST::NodePtr Parser::parse_function_call_or_literal() {
    if (std::holds_alternative<IntegerLiteral>(currentToken)) {
        auto literal = std::get<IntegerLiteral>(currentToken);
        consume_token();
        return std::make_unique<AST::IntegerLiteralNode>(literal);
    }
    else if (std::holds_alternative<FloatLiteral>(currentToken)) {
        auto literal = std::get<FloatLiteral>(currentToken);
        consume_token();
        return std::make_unique<AST::FloatLiteralNode>(literal);
    }
    else if (std::holds_alternative<Identifier>(currentToken)) {
        auto id_node = std::make_unique<AST::IdentifierNode>(
                std::move(
                        std::get<Identifier>(currentToken)
                )
        );
        consume_token();
        // Check if it's a function call
        if (is_current_token(Punctuation::OpenParen)) {
            return parse_function_call(std::move(id_node));
        } else {
            return id_node;
        }
    }
    else if (is_current_token(Keyword::Fn)) {
        auto function = parse_function();
        if (is_current_token(Punctuation::OpenParen)) {
            return parse_function_call(std::move(function));
        } else {
            return function;
        }
    }
    else if (is_current_token(Keyword::If)) {
        return parse_if_statement();
    }
    else if (is_current_token(Punctuation::OpenBrace)) {
        return parse_block();
    }
    else if (is_current_token(Punctuation::OpenParen)) {
        consume_token();

        auto subExpression = parse_expression();

        expect_current_token(Punctuation::CloseParen, "Expected a closing parenthesis");
        consume_token();

        return subExpression;
    }

    throw_syntax_error("Expected literal, function call, or variable reference.");
}

AST::FunctionCallPtr Parser::parse_function_call(AST::NodePtr function) {
    consume_token();  // Consume the opening parenthesis

    std::vector<AST::NodePtr> arguments;
    while (!is_current_token(Punctuation::CloseParen)) {
        arguments.push_back(parse_expression());
        if (is_current_token(Punctuation::Comma)) {
            consume_token();
        }
    }

    consume_token();

    return std::make_unique<AST::FunctionCall>(std::move(function), std::move(arguments));
}

AST::NodePtr Parser::parse_if_statement() {
    expect_current_token(Keyword::If, "Expected if keyword");
    consume_token();
    auto expression = parse_expression();
    auto block = parse_block();
    AST::BlockNodePtr elseBlock;

    if (is_current_token(Keyword::Else)) {
        consume_token();
        elseBlock = parse_block();
    }
    return std::make_unique<AST::IfNode>(std::move(expression), std::move(block), std::move(elseBlock));
}
