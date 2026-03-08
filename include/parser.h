#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "tokens.h"
#include "ast.h"

typedef struct {
    Lexer *lexer;
    token current_token;
    int has_error;
} Parser;

// Funciones de inicializacion y control
void init_parser(Parser *p, Lexer *l);
ASTNode* parse(Parser *p);
void advance(Parser *p);
int match(Parser *p, tokenType expected);
void parser_error(Parser *p, const char *message);
void synchronize(Parser *p);

// Reglas principales
ASTNode* parse_program(Parser *p);
ASTNode* parse_instruction(Parser *p);

// Instrucciones especificas
ASTNode* parse_declaration(Parser *p);
ASTNode* parse_assignment_or_unary(Parser *p);
ASTNode* parse_read(Parser *p);
ASTNode* parse_write(Parser *p);
ASTNode* parse_for(Parser *p);
ASTNode* parse_while(Parser *p);
ASTNode* parse_if(Parser *p);
ASTNode* parse_proc(Parser *p);

// Bloques y parametros
ASTNode* parse_block(Parser *p);
ASTNode* parse_parameters(Parser *p);

// Operaciones y expresiones lógicas/aritméticas
ASTNode* parse_logical_expression(Parser *p);
ASTNode* parse_unary_operation(Parser *p);
ASTNode* parse_expression(Parser *p);
ASTNode* parse_term(Parser *p);
ASTNode* parse_power(Parser *p);
ASTNode* parse_factor(Parser *p);

#endif