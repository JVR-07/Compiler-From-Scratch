#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "tokens.h"
#include "ast.h"

// Estructura principal del Parser
typedef struct {
    Lexer *lexer;
    token current_token;
    int has_error;
} Parser;

// Funciones de inicializacion y control
void init_parser(Parser *p, Lexer *l);
void parse(Parser *p);
void advance(Parser *p);
int match(Parser *p, tokenType expected);
void parser_error(Parser *p, const char *message);
void synchronize(Parser *p);

// Reglas principales
void parse_program(Parser *p);
void parse_instruction(Parser *p);

// Instrucciones especificas
void parse_declaration(Parser *p);
void parse_assignment_or_unary(Parser *p);
void parse_read(Parser *p);
void parse_write(Parser *p);
void parse_for(Parser *p);
void parse_while(Parser *p);
void parse_if(Parser *p);
void parse_proc(Parser *p);

// Bloques y parametros
void parse_block(Parser *p);
void parse_parameters(Parser *p);

// Operaciones y expresiones lógicas/aritméticas
void parse_logical_expression(Parser *p);
void parse_unary_operation(Parser *p);
ASTNode* parse_expression(Parser *p);
ASTNode* parse_term(Parser *p);
ASTNode* parse_factor(Parser *p);

#endif