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

void gen_cond_jump(ASTNode *cond, int lbl_false);
void gen_jump(int lbl);
void gen_label(int lbl);

#define MAX_PARAMS 6

void gen_proc_frame_enter();
void gen_proc_frame_exit();
void gen_proc_prologue(const char *name);
void gen_proc_epilogue();
void gen_proc_param(const char *name, tokenType type, int int_idx, int float_idx);

void register_proc(const char *name, tokenType *param_types, int param_count);
int  get_proc_param_count(const char *name);
tokenType get_proc_param_type(const char *name, int idx);

void gen_proc_call(const char *name, ASTNode **args, int nargs);

#endif
