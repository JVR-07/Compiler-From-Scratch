#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

ASTNode* create_node(NodeType type, token t) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->t = t;
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode* create_binary_node(token op, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_node(NODE_BINARY_OP, op);
    node->left = left;
    node->right = right;
    return node;
}

void free_ast(ASTNode *node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

void print_ast(ASTNode *node, int level, char* side) {
    if (!node) return;

    for (int i = 0; i < level; i++) {
        if(i == 0) {
            printf("   ");
        } else {    
        printf("|  ");
        }
    }

    printf("%s", side);

    switch (node->type) {
        case NODE_BINARY_OP:
            printf("[Operador: %s]\n", node->t.lexeme);
            break;
        case NODE_LITERAL:
            printf("[Literal: %s]\n", node->t.lexeme);
            break;
        case NODE_IDENTIFIER:
            printf("[Identifier: %s]\n", node->t.lexeme);
            break;
        default:
            printf("[Nodo desconocido]\n");
    }

    print_ast(node->left, level+ 1, "|-L:");
    print_ast(node->right, level+ 1, "|-R:");
}