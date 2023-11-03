//
// Created by Arvid Jonasson on 2023-10-08.
//

#ifndef COMPILER_ASTNODE_H
#define COMPILER_ASTNODE_H

#include <memory>
#include <utility>
#include <vector>
#include <optional>
#include "Token.h"
#include <ostream>

namespace AST {
    struct Node;

    using NodePtr = std::unique_ptr<Node>;

    struct Node {
        virtual ~Node() = default;
        virtual void transpile(std::ostream &) = 0;
    };

    struct IntegerLiteralNode : public Node {
        IntegerLiteral value;

        explicit IntegerLiteralNode(IntegerLiteral value) : value(value) {}

        void transpile(std::ostream &out) override;
    };

    using IntegerLiteralNodePtr = std::unique_ptr<IntegerLiteralNode>;

    struct IdentifierNode : public Node {
        Identifier identifier;

        explicit IdentifierNode(Identifier identifier) : identifier(std::move(identifier)) {}

        void transpile(std::ostream &out) override;
    };

    using IdentifierNodePtr = std::unique_ptr<IdentifierNode>;


    struct BinaryOpNode : public Node {
        Operator op;
        NodePtr left, right;

        BinaryOpNode(Operator op, NodePtr left, NodePtr right):
        op(op), left(std::move(left)), right(std::move(right)) {}


        void transpile(std::ostream &out) override;

    };

    using BinaryOpNodePtr = std::unique_ptr<BinaryOpNode>;

    struct FunctionNode : public Node {
        Identifier name;
        std::vector<Identifier> parameters;
        std::vector<NodePtr> statements;

        FunctionNode(Identifier name, std::vector<Identifier> parameters, std::vector<NodePtr> statements) :
        name(std::move(name)), parameters(std::move(parameters)), statements(std::move(statements)) {}

        void transpile(std::ostream &out) override;
    };

    using FunctionNodePtr = std::unique_ptr<FunctionNode>;

    struct IfNode : public Node {
        NodePtr expression;
        NodePtr statement;

        // nullptr if no else block
        NodePtr elseStatement;

        IfNode(NodePtr expression, NodePtr statement, NodePtr elseStatement):
        expression(std::move(expression)), statement(std::move(statement)), elseStatement(std::move(elseStatement)) {}

        void transpile(std::ostream &out) override;
    };

    using IfNodePtr = std::unique_ptr<IfNode>;

    struct DeclarationNode : public Node {
        Identifier name;
        // The expression the declaration equals
        NodePtr expression;

        DeclarationNode(Identifier name, NodePtr expression):
        name(std::move(name)), expression(std::move(expression)) {}

        void transpile(std::ostream &out) override;
    };

    using DeclarationNodePtr = std::unique_ptr<DeclarationNode>;

    struct ReturnNode : public Node {
        NodePtr expression;

        explicit ReturnNode(NodePtr expression) : expression(std::move(expression)) {}

        void transpile(std::ostream &out) override;
    };

    using ReturnNodePtr = std::unique_ptr<ReturnNode>;

    struct FunctionCall : public Node {
        Identifier identifier;
        std::vector<NodePtr> arguments;

        FunctionCall(Identifier identifier, std::vector<NodePtr> arguments):
        identifier(std::move(identifier)), arguments(std::move(arguments)) {}

        void transpile(std::ostream &out) override;
    };

    using FunctionCallPtr = std::unique_ptr<FunctionCall>;
}


#endif //COMPILER_ASTNODE_H
