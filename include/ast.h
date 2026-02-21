#ifndef AST_H
#define AST_H

#include "tokens.h"

typedef enum {
    NODE_BINARY_OP,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_UNARY_OP
} NodeType;

typedef struct ASTNode {
    NodeType type;
    token t;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode* create_node(NodeType type, token t);
ASTNode* create_binary_node(token op, ASTNode *left, ASTNode *right);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int level, char* side);

#endif