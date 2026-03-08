#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "tokens.h"

typedef struct {
    int id;
    char lexeme[100];
    tokenType type;      // Para TKN_IDENTIFIER
    tokenType data_type; // Para el tipo semantico: TKN_INT, TKN_FLOAT, etc.
    int scope_level;
} symbol;

void init_symbol_table();

void enter_scope();
void exit_scope();

int install_symbol(const char *lexeme, tokenType type, tokenType data_type);
int lookup_symbol(const char *lexeme);
tokenType get_symbol_type(int id);
void print_symbol_table();

#endif