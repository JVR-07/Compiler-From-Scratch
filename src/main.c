#include <stdio.h>
#include "lexer.h"
#include "symbols.h"
#include "parser.h"

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

    Parser parser;
    init_parser(&parser, &lexer);
        
    parse(&parser);

    print_symbol_table();

    close_lexer(&lexer);
    fclose(f);
    
    return 0;
}