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

namespace AST {
    struct Node;

    using NodePtr = std::unique_ptr<Node>;
    struct Variable {
        Identifier name;
        std::optional<Type> type;
    };
    struct Parameter {
        Identifier name;
        std::optional<Type> type;
    };
    using ParameterList = std::vector<Parameter>;
    struct Node {
        virtual ~Node() = default;
    };

    struct IntegerLiteralNode: public Node {
        IntegerLiteral value;

        explicit IntegerLiteralNode(IntegerLiteral value): value(value) {}
    };
    using IntegerLiteralNodePtr = std::unique_ptr<IntegerLiteralNode>;

    struct FloatLiteralNode: public Node {
        FloatLiteral value;

        explicit FloatLiteralNode(FloatLiteral value): value(value) {}
    };
    using FloatLiteralNodePtr = std::unique_ptr<FloatLiteralNode>;

    struct IdentifierNode: public Node {
        Identifier name;

        explicit IdentifierNode(Identifier name): name(std::move(name)) {}
    };
    using IdentifierNodePtr = std::unique_ptr<IdentifierNode>;

    struct BinaryOpNode: public Node {
        Operator op;
        NodePtr left, right;

        BinaryOpNode(Operator op, NodePtr left, NodePtr right):
        op(op), left(std::move(left)), right(std::move(right)) {}
    };
    using BinaryOpNodePtr = std::unique_ptr<BinaryOpNode>;

    struct BlockNode: public Node {
        std::vector<NodePtr> statements;

        void addStatement(NodePtr statement) {
            statements.emplace_back(std::move(statement));
        }
    };
    using BlockNodePtr = std::unique_ptr<BlockNode>;

    struct ExpressionBlockNode: public Node {
        BlockNodePtr block;
    };
    using ExpressionBlockNodePtr = std::unique_ptr<ExpressionBlockNode>;

    struct FunctionNode: public Node {
        ParameterList parameters;
        std::optional<Type> returnType;
        BlockNodePtr block;

        FunctionNode(ParameterList parameters, std::optional<Type> returnType, BlockNodePtr block):
        parameters(std::move(parameters)), returnType(returnType), block(std::move(block)) {}
    };
    using FunctionNodePtr = std::unique_ptr<FunctionNode>;

    struct FunctionType {
        std::vector<Type> parameterTypes;
        std::optional<Type> returnType;
    };

    struct FunctionDeclarationNode: public Node {
        Identifier name;
        FunctionType functionType;
        FunctionNodePtr functionNode;
        FunctionDeclarationNode(Identifier name, FunctionType functionType, FunctionNodePtr functionNode):
        name(std::move(name)), functionType(std::move(functionType)), functionNode(std::move(functionNode)) {}
    };

    using FunctionDeclarationNodePtr = std::unique_ptr<FunctionDeclarationNode>;

    struct IfNode: public Node {
        NodePtr expression;
        // This block should be of type BlockExpression since the user can return from it using '<-'
        // The '<-' operator will be emulated in C++ using throw BlockReturn(val);
        // And the BlockExpression should therefore be a lambda with a try catch block inside it, like:
        //  [&]() {
        //      try {
        //          ...
        //      } catch (const BlockReturn& e) {
        //          return e.val;
        //      }
        //  }();
        //
        NodePtr block;

        IfNode(NodePtr expression, NodePtr block): expression(std::move(expression)), block(std::move(block)) {}
    };
    using IfNodePtr = std::unique_ptr<IfNode>;

    struct WhileNode: public Node {
        NodePtr expression;
        NodePtr block;

        WhileNode(NodePtr expression, NodePtr block): expression(std::move(expression)), block(std::move(block)) {}
    };
    using WhileNodePtr = std::unique_ptr<WhileNode>;

    struct VariableDeclarationNode: public Node {
        Variable variable;
        NodePtr expression;

        VariableDeclarationNode(Variable variable, NodePtr expression):
        variable(std::move(variable)), expression(std::move(expression)) {}
    };
    using VariableDeclarationNodePtr = std::unique_ptr<VariableDeclarationNode>;

    struct ReturnNode: public Node {
        NodePtr expression;

        explicit ReturnNode(NodePtr expression): expression(std::move(expression)) {}
    };
    using ReturnNodePtr = std::unique_ptr<ReturnNode>;

}



#endif //COMPILER_ASTNODE_H
