//
// Created by Arvid Jonasson on 2023-10-08.
//

#include "ASTNode.h"
#include <unordered_map>

using namespace AST;

static std::unordered_map<Operator, std::string> operators {
        // Arithmetic operators
        {Operator::Add, "+"},
        {Operator::Subtract, "-"},
        {Operator::Multiply, "*"},
        {Operator::Divide, "/"},
        {Operator::Modulus, "%"},

        // Assignment operator
        {Operator::Assignment, "="},

        // Relational operators
        {Operator::Equal, "=="},
        {Operator::NotEqual, "!="},
        {Operator::LessThan, "<"},
        {Operator::GreaterThan, ">"},
        {Operator::LessThanOrEq, "<="},
        {Operator::GreaterThanOrEq, ">="},

        // Logical operators
        {Operator::LogicalAnd, "&&"},
        {Operator::LogicalOr, "||"},
        {Operator::LogicalNot, "!"},
};

void AST::BinaryOpNode::transpile(std::ostream &out) {
    out << "((";
    left->transpile(out);
    out << ')' << (operators[op]) << '(';
    right->transpile(out);
    out << "))";
}

void AST::IntegerLiteralNode::transpile(std::ostream &out) {
    out << "(" << value << ")";
}

void AST::IdentifierNode::transpile(std::ostream &out) {
    out << "(" << identifier << ")";
}

void AST::FunctionNode::transpile(std::ostream &out) {
    out << "int " << name << '(';
    for(std::size_t i = 1; const auto &parameter : parameters) {
        out << "int " << parameter;
        if(i != parameters.size())
            out << ", ";
        ++i;
    }
    out << ") {\n";
    for(const auto &statement : statements) {
        statement->transpile(out);
        out << ";\n";
    }
    out << "}\n";
}

void AST::IfNode::transpile(std::ostream &out) {
    out << "if (";
    expression->transpile(out);
    out << ") ";
    statement->transpile(out);
    if(elseStatement) {
        out << ";\nelse (";
        elseStatement->transpile(out);
        out << ")";
    }
}

void DeclarationNode::transpile(std::ostream &out) {
    out << "int " << name << " = (";
    expression->transpile(out);
    out << ")";
}

void ReturnNode::transpile(std::ostream &out) {
    out << "return (";
    expression->transpile(out);
    out << ")";
}

void FunctionCall::transpile(std::ostream &out) {
    out << identifier << "(";
    for(std::size_t i = 1; const auto &arg : arguments) {
        arg->transpile(out);
        if(i != arguments.size())
            out << ", ";
        ++i;
    }
    out << ")";
}
