#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "codegen.h"
#include "tokens.h"
#define MAX_VARS  200
#define MAX_PROCS 20

typedef struct {
    char      name[100];
    int       offset;
    tokenType type;
} VarEntry;

typedef struct {
    char      name[100];
    tokenType param_types[MAX_PARAMS];
    int       param_count;
} ProcEntry;

static VarEntry  var_table[MAX_VARS];
static int       var_count   = 0;
static int       current_off = 0;
static int       label_count = 0;

static ProcEntry proc_table[MAX_PROCS];
static int       proc_count  = 0;

static VarEntry  saved_var_table[MAX_VARS];
static int       saved_var_count  = 0;
static int       saved_current_off = 0;

int codegen_errors = 0;


void codegen_error(int line, const char *msg, const char *detail) {
    if (line > 0)
        fprintf(stderr, "\n\033[1;31mError Codegen\033[0m [Línea %d]: %s", line, msg);
    else
        fprintf(stderr, "\n\033[1;31mError Codegen\033[0m: %s", msg);

    if (detail && detail[0] != '\0')
        fprintf(stderr, " '%s'", detail);

    fprintf(stderr, "\n");
    codegen_errors++;
}

void emit(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}


int new_label() {
    return ++label_count;
}

int alloc_var(const char *name, tokenType type) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0) {
            return var_table[i].offset;
        }
    }

    if (var_count >= MAX_VARS) {
        codegen_error(0, "Tabla de variables de código desbordada.", name);
        return 0;
    }

    int size = (type == TKN_FLOAT) ? 8 : 4;
    current_off -= size;

    VarEntry entry;
    strncpy(entry.name, name, sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = '\0';
    entry.offset = current_off;
    entry.type   = type;

    var_table[var_count++] = entry;
    return current_off;
}

int get_var_offset(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0)
            return var_table[i].offset;
    }
    return 0; 
}

tokenType get_var_type(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_table[i].name, name) == 0)
            return var_table[i].type;
    }
    return TKN_ERROR;
}

void init_codegen() {
    emit(".intel_syntax noprefix");
    emit("");
    emit("    .section .data");
    emit("fmt_int:");
    emit("    .string \"%%d\\n\"");
    emit("fmt_float:");
    emit("    .string \"%%.6f\\n\"");
    emit("fmt_str:");
    emit("    .string \"%%s\\n\"");
    emit("fmt_bool:");
    emit("    .string \"%%d\\n\"");
    emit("fmt_read_int:");
    emit("    .string \"%%d\"");
    emit("fmt_read_float:");
    emit("    .string \"%%lf\"");
    emit("");
    emit("    .section .text");
    emit("    .global main");
    emit("");
    emit("main:");
    emit("    push    rbp");
    emit("    mov     rbp, rsp");
    emit("    sub     rsp, 512");
    emit("");
    emit("    # === inicio del programa ===");
    emit("");
}

void finalize_codegen() {
    emit("");
    emit("    # === fin del programa ===");
    emit("    mov     eax, 0");
    emit("    leave");
    emit("    ret");
}

static int is_float(tokenType t) { return t == TKN_FLOAT; }

void gen_cond_jump(ASTNode *cond, int lbl_false) {
    gen_expr(cond);
    emit("    cmp     eax, 0");
    emit("    je      .L%d", lbl_false);
}

void gen_jump(int lbl) {
    emit("    jmp     .L%d", lbl);
}

void gen_label(int lbl) {
    emit(".L%d:", lbl);
}

static const char *int_arg_regs32[] = {"edi","esi","edx","ecx","r8d","r9d"};
static const char *int_arg_regs64[] = {"rdi","rsi","rdx","rcx","r8", "r9" };

void register_proc(const char *name, tokenType *param_types, int param_count) {
    if (proc_count >= MAX_PROCS) return;
    ProcEntry *e = &proc_table[proc_count++];
    strncpy(e->name, name, 99); e->name[99] = '\0';
    e->param_count = param_count < MAX_PARAMS ? param_count : MAX_PARAMS;
    for (int i = 0; i < e->param_count; i++) e->param_types[i] = param_types[i];
}

int get_proc_param_count(const char *name) {
    for (int i = 0; i < proc_count; i++)
        if (strcmp(proc_table[i].name, name) == 0) return proc_table[i].param_count;
    return -1;
}

tokenType get_proc_param_type(const char *name, int idx) {
    for (int i = 0; i < proc_count; i++)
        if (strcmp(proc_table[i].name, name) == 0) return proc_table[i].param_types[idx];
    return TKN_ERROR;
}

void gen_proc_frame_enter() {
    memcpy(saved_var_table, var_table, sizeof(VarEntry) * var_count);
    saved_var_count   = var_count;
    saved_current_off = current_off;
    var_count   = 0;
    current_off = 0;
}

void gen_proc_frame_exit() {
    memcpy(var_table, saved_var_table, sizeof(VarEntry) * saved_var_count);
    var_count   = saved_var_count;
    current_off = saved_current_off;
}

void gen_proc_prologue(const char *name) {
    emit("%s:", name);
    emit("    push    rbp");
    emit("    mov     rbp, rsp");
    emit("    sub     rsp, 512");
}

void gen_proc_epilogue() {
    emit("    leave");
    emit("    ret");
}

void gen_proc_param(const char *name, tokenType type, int int_idx, int float_idx) {
    int offset = alloc_var(name, type);
    if (type == TKN_FLOAT) {
        emit("    movsd   QWORD PTR [rbp%d], xmm%d", offset, float_idx);
    } else {
        if (int_idx >= MAX_PARAMS) {
            codegen_error(0, "Demasiados parametros enteros (max 6).", name);
            return;
        }
        emit("    mov     DWORD PTR [rbp%d], %s", offset, int_arg_regs32[int_idx]);
    }
}

void gen_proc_call(const char *name, ASTNode **args, int nargs) {
    int n = get_proc_param_count(name);
    if (n < 0) { codegen_error(0, "Proc no registrado:", name); return; }
    if (nargs != n) { codegen_error(0, "Numero incorrecto de argumentos al llamar:", name); return; }

    for (int i = 0; i < nargs; i++) {
        tokenType ptype = get_proc_param_type(name, i);
        gen_expr(args[i]);
        if (ptype == TKN_FLOAT) { emit("    sub     rsp, 8"); emit("    movsd   QWORD PTR [rsp], xmm0"); }
        else                    { emit("    push    rax"); }
    }

    int ii = 0, fi = 0;
    for (int i = nargs - 1; i >= 0; i--) {
        tokenType ptype = get_proc_param_type(name, i);
        if (ptype == TKN_FLOAT) { emit("    movsd   xmm%d, QWORD PTR [rsp]", fi++); emit("    add     rsp, 8"); }
        else                    { emit("    pop     %s", int_arg_regs64[ii++]); }
    }
    emit("    call    %s", name);
}


char* gen_expr(ASTNode *node) {
    if (!node) {
        codegen_error(0, "Nodo AST nulo en gen_expr.", NULL);
        emit("    # ERROR: nodo nulo");
        return "eax";
    }

    switch (node->type) {

        case NODE_LITERAL: {
            switch (node->t.type) {
                case TKN_LIT_INT:
                    emit("    mov     eax, %s", node->t.lexeme);
                    return "eax";

                case TKN_LIT_FLOAT: {
                    int lbl = new_label();
                    emit("    .section .rodata");
                    emit(".flt_%d:", lbl);
                    emit("    .double %s", node->t.lexeme);
                    emit("    .section .text");
                    emit("    movsd   xmm0, QWORD PTR [rip + .flt_%d]", lbl);
                    return "xmm0";
                }

                case TKN_LIT_TRUE:
                    emit("    mov     eax, 1");
                    return "eax";

                case TKN_LIT_FALSE:
                    emit("    mov     eax, 0");
                    return "eax";

                case TKN_LIT_STRING: {
                    int lbl = new_label();
                    emit("    .section .rodata");
                    emit(".str_%d:", lbl);
                    emit("    .string %s", node->t.lexeme);
                    emit("    .section .text");
                    emit("    lea     rax, [rip + .str_%d]", lbl);
                    return "rax";
                }

                default:
                    codegen_error(node->t.line, "Literal no soportado.", node->t.lexeme);
                    emit("    # ERROR: literal no soportado");
                    return "eax";
            }
        }

        case NODE_IDENTIFIER: {
            tokenType type = get_var_type(node->t.lexeme);
            int offset     = get_var_offset(node->t.lexeme);

            if (type == TKN_ERROR) {
                codegen_error(node->t.line, "Variable no registrada en codegen:", node->t.lexeme);
                emit("    # ERROR: variable '%s' no registrada", node->t.lexeme);
                emit("    mov     eax, 0");
                return "eax";
            }

            if (is_float(type)) {
                emit("    movsd   xmm0, QWORD PTR [rbp%d]", offset);
                return "xmm0";
            }
            emit("    mov     eax, DWORD PTR [rbp%d]", offset);
            return "eax";
        }

        case NODE_ASSIGN: {
            if (!node->left || !node->right) {
                codegen_error(node->t.line, "Nodo de asignacion incompleto.", NULL);
                return "eax";
            }

            gen_expr(node->right);

            const char *var_name = node->left->t.lexeme;
            tokenType type       = get_var_type(var_name);
            int offset           = get_var_offset(var_name);

            if (type == TKN_ERROR) {
                codegen_error(node->t.line, "Variable destino no encontrada:", var_name);
                return "eax";
            }

            if (is_float(type))
                emit("    movsd   QWORD PTR [rbp%d], xmm0", offset);
            else
                emit("    mov     DWORD PTR [rbp%d], eax", offset);

            return "eax";
        }

        case NODE_BINARY_OP: {
            if (!node->left || !node->right) {
                codegen_error(node->t.line, "Operacion binaria con hijos nulos.", NULL);
                return "eax";
            }

            int use_float = is_float(node->eval_type)        ||
                            is_float(node->left->eval_type)   ||
                            is_float(node->right->eval_type);

            if (use_float) {
                gen_expr(node->left);
                emit("    sub     rsp, 8");
                emit("    movsd   QWORD PTR [rsp], xmm0");
                gen_expr(node->right);
                emit("    movsd   xmm1, xmm0");
                emit("    movsd   xmm0, QWORD PTR [rsp]");
                emit("    add     rsp, 8");

                switch (node->t.type) {
                    case TKN_PLUS:  emit("    addsd   xmm0, xmm1"); break;
                    case TKN_MINUS: emit("    subsd   xmm0, xmm1"); break;
                    case TKN_MULT:  emit("    mulsd   xmm0, xmm1"); break;
                    case TKN_DIV:   emit("    divsd   xmm0, xmm1"); break;
                    case TKN_POWER: emit("    call    pow");         break;
                    case TKN_EQUAL:
                    case TKN_NOT_EQUAL:
                    case TKN_LESS:
                    case TKN_LESS_EQUAL:
                    case TKN_GREATER:
                    case TKN_GREATER_EQUAL:
                        emit("    ucomisd xmm0, xmm1");
                        if (node->t.type == TKN_EQUAL)           emit("    sete    al");
                        else if (node->t.type == TKN_NOT_EQUAL)  emit("    setne   al");
                        else if (node->t.type == TKN_LESS)       emit("    setb    al");
                        else if (node->t.type == TKN_LESS_EQUAL) emit("    setbe   al");
                        else if (node->t.type == TKN_GREATER)    emit("    seta    al");
                        else if (node->t.type == TKN_GREATER_EQUAL) emit("    setae   al");
                        emit("    movzx   eax, al");
                        return "eax";
                    default:
                        codegen_error(node->t.line, "Operador float no soportado:", node->t.lexeme);
                }
                return "xmm0";

            } else {
                gen_expr(node->left);
                emit("    push    rax");
                gen_expr(node->right);
                emit("    mov     ebx, eax");
                emit("    pop     rax");

                switch (node->t.type) {
                    case TKN_PLUS:
                        emit("    add     eax, ebx");
                        break;

                    case TKN_MINUS:
                        emit("    sub     eax, ebx");
                        break;

                    case TKN_MULT:
                        emit("    imul    eax, ebx");
                        break;

                    case TKN_DIV: {
                        if (node->right->type == NODE_LITERAL &&
                            strcmp(node->right->t.lexeme, "0") == 0) {
                            codegen_error(node->t.line, "Division por cero detectada.", NULL);
                            emit("    # ERROR: division por cero");
                            break;
                        }
                        emit("    cdq");
                        emit("    idiv    ebx");
                        break;
                    }

                    case TKN_POWER: {
                        int lbl_loop = new_label();
                        int lbl_end  = new_label();
                        emit("    mov     ecx, ebx");
                        emit("    mov     edx, eax");
                        emit("    mov     eax, 1");
                        emit("    cmp     ecx, 0");
                        emit("    je      .L%d", lbl_end);
                        emit(".L%d:", lbl_loop);
                        emit("    imul    eax, edx");
                        emit("    dec     ecx");
                        emit("    jnz     .L%d", lbl_loop);
                        emit(".L%d:", lbl_end);
                        break;
                    }

                    case TKN_EQUAL:
                    case TKN_NOT_EQUAL:
                    case TKN_LESS:
                    case TKN_LESS_EQUAL:
                    case TKN_GREATER:
                    case TKN_GREATER_EQUAL:
                        emit("    cmp     eax, ebx");
                        if (node->t.type == TKN_EQUAL)           emit("    sete    al");
                        else if (node->t.type == TKN_NOT_EQUAL)  emit("    setne   al");
                        else if (node->t.type == TKN_LESS)       emit("    setl    al");
                        else if (node->t.type == TKN_LESS_EQUAL) emit("    setle   al");
                        else if (node->t.type == TKN_GREATER)    emit("    setg    al");
                        else if (node->t.type == TKN_GREATER_EQUAL) emit("    setge   al");
                        emit("    movzx   eax, al");
                        break;

                    case TKN_AND:
                        emit("    and     eax, ebx");
                        break;
                    case TKN_OR:
                        emit("    or      eax, ebx");
                        break;

                    default:
                        codegen_error(node->t.line, "Operador no soportado:", node->t.lexeme);
                        emit("    # ERROR: operador no soportado");
                }
                return "eax";
            }
        }

        case NODE_UNARY_OP: {
            if (node->t.type == TKN_SELF_PLUS || node->t.type == TKN_SELF_MINUS) {
                if (!node->left) {
                    codegen_error(node->t.line, "Operacion unaria sin operando.", NULL);
                    return "eax";
                }
                const char *var_name = node->left->t.lexeme;
                int offset           = get_var_offset(var_name);
                tokenType type       = get_var_type(var_name);

                if (type == TKN_ERROR) {
                    codegen_error(node->t.line, "Variable no encontrada para ++/--:", var_name);
                    return "eax";
                }

                if (node->t.type == TKN_SELF_PLUS)
                    emit("    add     DWORD PTR [rbp%d], 1", offset);
                else
                    emit("    sub     DWORD PTR [rbp%d], 1", offset);

            } else if (node->t.type == TKN_NOT) {
                if (!node->left) {
                    codegen_error(node->t.line, "Operacion NOT sin operando.", NULL);
                    return "eax";
                }
                gen_expr(node->left);
                emit("    cmp     eax, 0");
                emit("    sete    al");
                emit("    movzx   eax, al");

            } else {
                codegen_error(node->t.line, "Operador unario no reconocido:", node->t.lexeme);
                emit("    # ERROR: operador unario no reconocido");
            }
            return "eax";
        }

        case NODE_WRITE: {
            if (!node->left) return "eax";
            gen_expr(node->left);
            tokenType type = node->left->eval_type;
            
            if (type == TKN_INT) {
                emit("    mov     esi, eax");
                emit("    lea     rdi, [rip + fmt_int]");
                emit("    mov     eax, 0");
                emit("    call    printf");
            } else if (type == TKN_FLOAT) {
                emit("    lea     rdi, [rip + fmt_float]");
                emit("    mov     eax, 1");
                emit("    call    printf");
            } else if (type == TKN_BOOL) {
                emit("    mov     esi, eax");
                emit("    lea     rdi, [rip + fmt_bool]");
                emit("    mov     eax, 0");
                emit("    call    printf");
            } else if (type == TKN_STRING) {
                emit("    mov     rsi, rax");
                emit("    lea     rdi, [rip + fmt_str]");
                emit("    mov     eax, 0");
                emit("    call    printf");
            }
            return "eax";
        }

        case NODE_READ: {
            if (!node->left || node->left->type != NODE_IDENTIFIER) return "eax";
            const char* var_name = node->left->t.lexeme;
            tokenType type = get_var_type(var_name);
            int offset = get_var_offset(var_name);

            if (type == TKN_ERROR) {
                codegen_error(node->t.line, "Variable no registrada en codegen:", var_name);
                return "eax";
            }

            if (type == TKN_INT || type == TKN_BOOL) {
                emit("    lea     rsi, [rbp%d]", offset);
                emit("    lea     rdi, [rip + fmt_read_int]");
                emit("    mov     eax, 0");
                emit("    call    scanf");
            } else if (type == TKN_FLOAT) {
                emit("    lea     rsi, [rbp%d]", offset);
                emit("    lea     rdi, [rip + fmt_read_float]");
                emit("    mov     eax, 0");
                emit("    call    scanf");
            } else {
                codegen_error(node->t.line, "Lectura no soportada para este tipo.", var_name);
            }
            return "eax";
        }

        default:
            emit("    # TODO: nodo tipo %d pendiente de implementar", node->type);
            return "eax";
    }
}