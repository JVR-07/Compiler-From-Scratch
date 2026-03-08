#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

ASTNode* create_node(NodeType type, token t) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->t = t;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->condition = NULL;
    node->increment = NULL;
    node->body = NULL;
    node->eval_type = TKN_EOF; // Default genérico
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
    free_ast(node->condition);
    free_ast(node->increment);
    free_ast(node->body);
    free_ast(node->next);
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
        case NODE_BINARY_OP:    printf("[Operador: %s]\n", node->t.lexeme); break;
        case NODE_LITERAL:      printf("[Literal: %s]\n", node->t.lexeme); break;
        case NODE_IDENTIFIER:   printf("[Identifier: %s]\n", node->t.lexeme); break;
        case NODE_UNARY_OP:     printf("[Unario: %s]\n", node->t.lexeme); break;
        case NODE_PROGRAM:      printf("[Program]\n"); break;
        case NODE_VAR_DECL:     printf("[Declaracion Var: %s]\n", node->t.lexeme); break;
        case NODE_ASSIGN:       printf("[Asignacion]\n"); break;
        case NODE_IF:           printf("[If]\n"); break;
        case NODE_WHILE:        printf("[While]\n"); break;
        case NODE_FOR:          printf("[For]\n"); break;
        case NODE_PROC_DECL:    printf("[Proc Decl: %s]\n", node->t.lexeme); break;
        case NODE_PROC_CALL:    printf("[Call: %s]\n", node->t.lexeme); break;
        case NODE_BLOCK:        printf("[Bloque]\n"); break;
        case NODE_READ:         printf("[Read]\n"); break;
        case NODE_WRITE:        printf("[Write]\n"); break;
        default:                printf("[Nodo desconocido]\n");
    }

    if (node->left) print_ast(node->left, level+ 1, "|-L:");
    if (node->right) print_ast(node->right, level+ 1, "|-R:");
    if (node->condition) print_ast(node->condition, level+ 1, "|-Cond:");
    if (node->increment) print_ast(node->increment, level+ 1, "|-Inc:");
    if (node->body) print_ast(node->body, level+ 1, "|-Body:");
    if (node->next) print_ast(node->next, level, "");
}