#include "ast.h"
#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "symbols.h"
#include <stdio.h>

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

  init_codegen();

  Parser parser;
  init_parser(&parser, &lexer);

  parse_program(&parser);

  finalize_codegen();

  // print_symbol_table();  // COMENTADO: solo mostrar ensamblador

  close_lexer(&lexer);
  fclose(f);

  if (codegen_errors > 0) {
    fprintf(stderr,
            "\n\033[1;33mGeneraci\u00f3n abortada\033[0m: %d error(es) de "
            "c\u00f3digo encontrados.\n",
            codegen_errors);
    return 1;
  }

  return 0;
}