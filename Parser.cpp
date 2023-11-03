//
// Created by Arvid Jonasson on 2023-10-06.
//

#include "Parser.h"

Parser &Parser::parse_program() {
    consume_token();
    while (!holds_alternative<EndToken>(currentToken)) {
        functions.emplace_back(std::move(parse_function()));
    }
    if(!decl_funcs.contains("main"))
        throw_syntax_error("There is no main declared");
    if(decl_funcs["main"] != 0)
        throw_syntax_error("Main shouldn't have any arguments.");

    return *this;
}


AST::DeclarationNodePtr Parser::parse_declaration() {
    expect_current_token(Keyword::Let, "Expected 'Let' keyword to declare variable");

    Identifier name = std::move(get_expected_or_throw<Identifier>("Expected function name"));

    if(decl_funcs.contains(name) || decl_vars.contains(name))
        throw_syntax_error(name + " is already declared");
    decl_vars.insert(name);

    expect_next_token(Operator::Assignment, "Expected assignment operator after variable declaration");

    consume_token();

    auto expression = parse_expression();

    expect_current_token(Punctuation::Semicolon, "Expected semicolon after variable declaration");
    return std::make_unique<AST::DeclarationNode>(std::move(name), std::move(expression));
}

AST::FunctionNodePtr Parser::parse_function() {
    expect_current_token(Keyword::Fn, "Expected the fn keyword to declare the function");

    Identifier name = std::move(get_expected_or_throw<Identifier>("Expected function name"));

    if(decl_funcs.contains(name))
        throw_syntax_error(name + " is already declared");

    consume_token();

    auto parameterList = parse_parameter_list();

    decl_funcs[name] = parameterList.size();

    decl_vars.clear();

    for(const auto &parameter : parameterList) {
        if(decl_vars.contains(parameter))
            throw_syntax_error(parameter + " is already declared.");
        decl_vars.insert(parameter);
    }

    expect_current_token(Punctuation::CloseParen, "Expected closing parenthesis");

    expect_next_token(Punctuation::OpenBrace, "Expected opening brace after function declaration");

    std::vector<AST::NodePtr> statements;
    consume_token();
    while (!is_current_token(Punctuation::CloseBrace)) {
        statements.emplace_back(std::move(parse_statement()));
        expect_current_token(Punctuation::Semicolon, "Expected semicolon after statement");
        consume_token();
    }

    if(statements.empty() || !dynamic_cast<AST::ReturnNode*>(statements.back().get()))
        throw_syntax_error(name + " doesn't end with a return statement");
    consume_token(); // Close brace

    return std::make_unique<AST::FunctionNode>(std::move(name), std::move(parameterList), std::move(statements));
}

AST::NodePtr Parser::parse_statement(bool declaration_allowed) {
    if (is_current_token(EndToken()))
        return {};
    // Figure out what the statement is through forward-looking method
    // Statement can be function declaration, variable declaration, if statement, while loop, expression
    AST::NodePtr ret;
    if (is_current_token(Keyword::If)) {
        ret = parse_if_statement();
    } else if (is_current_token(Keyword::Let)) {
        if(!declaration_allowed)
            throw_syntax_error("Declaration is not allowed here");
        ret = parse_declaration();
    } else if (is_current_token(Keyword::Return)) {
        ret = parse_return_statement();
    } else {
        ret = parse_expression();
    }
    expect_current_token(Punctuation::Semicolon, "Expected semicolon after statement");
    return ret;
}

std::vector<Identifier> Parser::parse_parameter_list() {
    std::vector<Identifier> parameterList;

    expect_current_token(Punctuation::OpenParen, "Expected opening parenthesis");
    currentToken = lexer.getNextToken();
    while (std::holds_alternative<Identifier>(currentToken)) {
        auto name = std::move(std::get<Identifier>(currentToken));
        parameterList.emplace_back(std::move(name));

        consume_token(); // identifier

        if(is_current_token(Punctuation::CloseParen))
            break;

        expect_current_token(Punctuation::Comma, "Expected comma, colon or closing parenthesis after variable declaration");
        consume_token();
    }
    expect_current_token(Punctuation::CloseParen, "Expected closing parenthesis.");
    return parameterList;
}

AST::NodePtr Parser::parse_expression() {
    return parse_or();
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
    else if (std::holds_alternative<Identifier>(currentToken)) {
        auto id_node = std::make_unique<AST::IdentifierNode>(
                std::move(
                        std::get<Identifier>(currentToken)
                )
        );
        consume_token();
        if (is_current_token(Punctuation::OpenParen)) {
            return parse_function_call(std::move(id_node));
        } else {
            if(!decl_vars.contains(id_node->identifier))
                throw_syntax_error(id_node->identifier + " is not declared");
            return id_node;
        }
    }
    else if (is_current_token(Keyword::If)) {
        return parse_if_statement();
    }
    else if (is_current_token(Punctuation::OpenParen)) {
        consume_token();

        auto subExpression = parse_expression();

        expect_current_token(Punctuation::CloseParen, "Expected a closing parenthesis");
        consume_token();

        return subExpression;
    }
    throw_syntax_error("Expected literal, function call, or variable reference");
    return {};
}

AST::FunctionCallPtr Parser::parse_function_call(AST::IdentifierNodePtr identifier) {
    if(!decl_funcs.contains(identifier->identifier))
        throw_syntax_error(identifier->identifier + " is not declared");
    consume_token();

    std::vector<AST::NodePtr> arguments;
    while (!is_current_token(Punctuation::CloseParen)) {
        arguments.emplace_back(std::move(parse_expression()));
        if (is_current_token(Punctuation::Comma)) {
            consume_token();
        }
    }

    if(decl_funcs[identifier->identifier] != arguments.size())
        throw_syntax_error("Argument count mismatch");

    consume_token();

    return std::make_unique<AST::FunctionCall>(std::move(identifier->identifier), std::move(arguments));
}

AST::NodePtr Parser::parse_if_statement() {
    expect_current_token(Keyword::If, "Expected if keyword");
    consume_token();
    auto expression = parse_expression();
    auto statement = parse_statement(false);
    AST::NodePtr elseStatement;

    if (is_current_token(Keyword::Else)) {
        consume_token();
        elseStatement = parse_statement(false);
    }
    expect_current_token(Punctuation::Semicolon, "Expected semicolon after statement");
    return std::make_unique<AST::IfNode>(std::move(expression), std::move(statement), std::move(elseStatement));
}

void Parser::transpile(std::ostream &out) {
    out << "#include <iostream>\n";
    out << "int print(int x) {std::cout << x << std::endl; return 0; }\n";
    for(const auto & function : functions) {
        function->transpile(out);
    }
}

AST::ReturnNodePtr Parser::parse_return_statement() {
    expect_current_token(Keyword::Return, "Expected return keyword.");
    consume_token();
    auto expression = parse_expression();
    expect_current_token(Punctuation::Semicolon, "Expected semicolon after statement");
    return std::make_unique<AST::ReturnNode>(std::move(expression));
}

AST::NodePtr Parser::parse_or() {
    auto left = parse_and();
    while (is_current_token(Operator::LogicalOr)) {
        auto op = std::get<Operator>(currentToken);
        consume_token();
        auto right = parse_and();
        left = std::make_unique<AST::BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

AST::NodePtr Parser::parse_and() {
    auto left = parse_equality();
    while (is_current_token(Operator::LogicalAnd)) {
        auto op = std::get<Operator>(currentToken);
        consume_token();
        auto right = parse_equality();
        left = std::make_unique<AST::BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

AST::NodePtr Parser::parse_equality() {
    auto left = parse_relational();
    while (is_current_token(Operator::Equal) || is_current_token(Operator::NotEqual)) {
        auto op = std::get<Operator>(currentToken);
        consume_token();
        auto right = parse_relational();
        left = std::make_unique<AST::BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

AST::NodePtr Parser::parse_relational() {
    auto left = parse_addition_subtraction();
    while (is_current_token(Operator::LessThan)
           || is_current_token(Operator::LessThanOrEq)
           || is_current_token(Operator::GreaterThan)
           || is_current_token(Operator::GreaterThanOrEq)) {
        auto op = std::get<Operator>(currentToken);
        consume_token();
        auto right = parse_addition_subtraction();
        left = std::make_unique<AST::BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

