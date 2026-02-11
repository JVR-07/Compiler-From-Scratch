#include <stdio.h>
#include "lexer.h"
#include "symbols.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo_fuente>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Error abriendo archivo");
        return 1;
    }

    Lexer lexer;
    init_lexer(&lexer, f);
    init_symbol_table();

    printf("Iniciando analisis lexico...\n");
    printf("%-20s | %-15s | %s\n", "NOMBRE DEL TOKEN", "VALOR ATRIBUTO", "LEXEMA REAL");
    printf("-------------------------------------------------------------------\n");

    token t;
    do {
        t = next_token(&lexer);
        
        if (t.type != TKN_EOF && t.type != TKN_ERROR) {
            
            // Solo Identificadores y Literales (numeros, strings)
            if (t.type == TKN_IDENTIFIER || 
                t.type == TKN_LIT_INT || 
                t.type == TKN_LIT_FLOAT || 
                t.type == TKN_LIT_STRING) {
                
                int id = install_symbol(t.lexeme, t.type);
                
                printf("%-20s | [%d]            | %s\n", 
                       token_type_to_str(t.type), 
                       id, 
                       t.lexeme);
            } else {
                // Tokens simples (IF, +, ;) se imprimen SIN atributo
                printf("%-20s |                | %s\n", 
                       token_type_to_str(t.type), 
                       t.lexeme);
            }
        } else if (t.type == TKN_ERROR) {
            printf("ERROR: Caracter desconocido en linea %d: %s\n", t.line, t.lexeme);
        } else if (t.type == TKN_EOF) {
            printf("Fin de archivo alcanzado.\n");
        }
        
    } while (t.type != TKN_EOF || t.type == TKN_ERROR);

    print_symbol_table();

    close_lexer(&lexer);
    fclose(f);
    
    return 0;
}