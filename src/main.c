#include <stdio.h>
#include "lexer.h"

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

    printf("Iniciando analisis lexico...\n");
    printf("%-20s %-20s %s\n", "TIPO", "LEXEMA", "LINEA");
    printf("--------------------------------------------------\n");

    token t;
    do {
        t = next_token(&lexer);
        
        if (t.type != TKN_EOF) {
            printf("%-20s %-20s %d\n", 
                   token_type_to_str(t.type), 
                   t.lexeme, 
                   t.line);
        }
        
    } while (t.type != TKN_EOF);

    close_lexer(&lexer);
    fclose(f);
    
    return 0;
}