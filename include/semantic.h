#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

void validate_node(ASTNode *node);
void analyze_semantic(ASTNode *root);

#endif
