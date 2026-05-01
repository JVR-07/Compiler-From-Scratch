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

// Control de flujo: usadas por el parser para if/while/for
// sin que el parser emita assembly directamente.
void gen_cond_jump(ASTNode *cond, int lbl_false);  // evalua cond, salta si es 0
void gen_jump(int lbl);                             // salto incondicional
void gen_label(int lbl);                            // emite etiqueta .Lx:

#endif
