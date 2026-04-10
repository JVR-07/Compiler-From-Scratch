#include <stdio.h>
#include <string.h>
#include "symbols.h"
#include "semantic.h"
#include "lexer.h"

#define MAX_SYMBOLS 100

symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;
int current_scope = 0;

void init_symbol_table() {
    symbol_count = 0;
    current_scope = 0;
}

void enter_scope() {
    current_scope++;
}

void exit_scope() {
    int new_count = 0;
    for (int i = 0; i < symbol_count; i++) {
        if (symbol_table[i].scope_level < current_scope) {
            symbol_table[new_count++] = symbol_table[i];
        }
    }
    symbol_count = new_count;

    if (current_scope > 0) {
        current_scope--;
    }
}


int lookup_symbol(const char *lexeme) {
    for (int i = symbol_count - 1; i >= 0; i--) {
        
        if (strcmp(symbol_table[i].lexeme, lexeme) == 0) {
            
            if (symbol_table[i].scope_level <= current_scope) {
                return i;
            }
        }
    }
    return -1;
}

int install_symbol(const char *lexeme, tokenType type, tokenType data_type) {
    for (int i = symbol_count - 1; i >= 0; i--) {
        
        if (symbol_table[i].scope_level < current_scope) break;
        
        if (strcmp(symbol_table[i].lexeme, lexeme) == 0 && symbol_table[i].scope_level == current_scope) {
            return symbol_table[i].id;
        }
    }

    if (symbol_count >= MAX_SYMBOLS) {
        semantic_error(0, "Tabla de símbolos llena.", NULL);
        return -1;
    }

    symbol new_sym;
    new_sym.id = symbol_count;

    strcpy(new_sym.lexeme, lexeme);

    new_sym.type = type;
    new_sym.data_type = data_type;
    new_sym.scope_level = current_scope;

    symbol_table[symbol_count] = new_sym;
    symbol_count++;

    return new_sym.id;
}

tokenType get_symbol_type(int id) {
    for (int i = 0; i < symbol_count; i++) {
        
        if (symbol_table[i].id == id) {
        
            return symbol_table[i].data_type;
        }
    }
    return TKN_ERROR;
}

void print_symbol_table() {
    printf("\n=== TABLA DE SIMBOLOS ===\n");
    printf("%-5s | %-20s | %-15s | %-6s\n", "ID", "LEXEMA", "TIPO DATO", "SCOPE");
    printf("----------------------------------------------------------\n");
    
    for (int i = 0; i < symbol_count; i++) {
        char type_str[50];
    
        if (symbol_table[i].type == TKN_IDENTIFIER) {
            strcpy(type_str, token_type_to_str(symbol_table[i].data_type));
        } else {
            strcpy(type_str, token_type_to_str(symbol_table[i].type));
        }

        printf("%-5d | %-20s | %-15s | %-6d\n", 
            symbol_table[i].id, 
            symbol_table[i].lexeme, 
            type_str,
            symbol_table[i].scope_level);
    }
    printf("----------------------------------------------------------\n");
}