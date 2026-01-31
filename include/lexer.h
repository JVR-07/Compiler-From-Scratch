#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "tokens.h"

typedef struct {
    FILE *source;
    char *line_buf;
    size_t line_cap;
    int line_pos;
    int line_num;
} Lexer;

void init_lexer(Lexer *l, FILE *f);
void close_lexer(Lexer *l);
token next_token(Lexer *l);
const char* token_type_to_str(tokenType t);

#endif