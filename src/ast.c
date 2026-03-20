#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

ASTNode* create_node(NodeType type, token t) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->t = t;
    node->left = NULL;
    node->right = NULL;
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
    free(node);
}

void print_ast(ASTNode *node, int level, char* side) {
    if (!node) return;

    // Imprimir el nivel de indentación con líneas verticales
    for (int i = 0; i < level; i++) {
        if (i == level - 1) {
            printf("|-- ");
        } else {    
            printf("|   ");
        }
    }

    // Si viene de una rama específica (L, R) imprimimos el prefijo corto
    if (side && side[0] != '\0') {
        printf("%s", side);
    }

    switch (node->type) {
        case NODE_BINARY_OP:    printf("[Operador: %s]\n", node->t.lexeme); break;
        case NODE_LITERAL:      printf("[Literal: %s]\n", node->t.lexeme); break;
        case NODE_IDENTIFIER:   printf("[Identificador: %s]\n", node->t.lexeme); break;
        case NODE_UNARY_OP:     printf("[Unario: %s]\n", node->t.lexeme); break;
        case NODE_ASSIGN:       printf("[Asignación]\n"); break;
        default:                printf("[Nodo desconocido]\n");
    }

    // Los hijos se indentan con etiquetas descriptivas cortas
    if (node->left)      print_ast(node->left, level + 1, "L: ");
    if (node->right)     print_ast(node->right, level + 1, "R: ");
}