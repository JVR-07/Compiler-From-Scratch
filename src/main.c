#include <stdio.h>
#include "lexer.h"
#include "symbols.h"
#include "parser.h"

#include "ast.h"
#include "semantic.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Formato: %s <archivo_fuente>\n", argv[0]);
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
        
    ASTNode* root = parse(&parser);
    
    if (!parser.has_error) {
        analyze_semantic(root);
    }

    print_symbol_table();
    free_ast(root);

    close_lexer(&lexer);
    fclose(f);
    
    return 0;
}