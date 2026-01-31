#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "tokens.h"

typedef struct {
    int id;
    char lexeme[100];
    tokenType type;
} symbol;

void init_symbol_table();

int install_symbol(const char *lexeme, tokenType type);
void print_symbol_table();

#endif