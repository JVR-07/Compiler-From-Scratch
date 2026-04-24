#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "tokens.h"

extern int codegen_errors;

void init_codegen();
void finalize_codegen();

char* gen_expr(ASTNode *node);

int alloc_var(const char *name, tokenType type);
int get_var_offset(const char *name);

tokenType get_var_type(const char *name);

int new_label();

void emit(const char *fmt, ...);

void codegen_error(int line, const char *msg, const char *detail);

#endif
