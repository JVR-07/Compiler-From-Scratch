#include <stdio.h>
#include <string.h>
#include "symbols.h"

#define MAX_SYMBOLS 100

symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

void init_symbol_table() {
    symbol_count = 0;
}

int install_symbol(const char *lexeme, tokenType type) {
    // Buscar si existe el simbolo
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].lexeme, lexeme) == 0) {
            return symbol_table[i].id; // Si existe, retornar ID
        }
    }

    // Si no existe, agregar el simbolo

    if (symbol_count >= MAX_SYMBOLS) { // Tabla llena
        printf("Error: Tabla de simbolos llena.\n");
        return -1;
    }

    symbol new_sym;
    new_sym.id = symbol_count;
    strcpy(new_sym.lexeme, lexeme);
    new_sym.type = type;

    symbol_table[symbol_count] = new_sym;
    symbol_count++;

    return new_sym.id;
}

void print_symbol_table() {
    printf("\n=== TABLA DE SIMBOLOS ===\n");
    printf("%-5s | %-20s | %-15s\n", "ID", "LEXEMA", "TIPO");
    printf("----------------------------------------------\n");
    for (int i = 0; i < symbol_count; i++) {
        // implementar funcion para convertir el enum en el string del tokentype
        printf("%-5d | %-20s | %d\n", 
            symbol_table[i].id, 
            symbol_table[i].lexeme, 
            symbol_table[i].type);
    }
    printf("----------------------------------------------\n");
}