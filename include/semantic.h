#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

void validate_node(ASTNode *node);
void analyze_semantic(ASTNode *root);
void semantic_error(int line, const char *message, const char *lexeme);

#endif
