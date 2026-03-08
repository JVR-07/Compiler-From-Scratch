#ifndef AST_H
#define AST_H

#include "tokens.h"

typedef enum {
    NODE_BINARY_OP,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_UNARY_OP,
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_PROC_DECL,
    NODE_PROC_CALL,
    NODE_BLOCK,
    NODE_READ,
    NODE_WRITE
} NodeType;

typedef struct ASTNode {
    NodeType type;
    token t;
    
    struct ASTNode *left;
    struct ASTNode *right;
    
    struct ASTNode *next;
    
    struct ASTNode *condition;
    struct ASTNode *increment;
    struct ASTNode *body;
    
    tokenType eval_type;  
} ASTNode;

ASTNode* create_node(NodeType type, token t);
ASTNode* create_binary_node(token op, ASTNode *left, ASTNode *right);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int level, char* side);

#endif