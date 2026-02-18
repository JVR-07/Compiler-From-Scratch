#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "symbols.h"

void init_parser(Parser *p, Lexer *l) {
    p->lexer = l;
    p->has_error = 0;
    advance(p); // Pre-carga del primer token
}

void advance(Parser *p) {
    p->current_token = next_token(p->lexer);
}

void parser_error(Parser *p, const char *message) {
    printf("Error Sintactico (Linea %d): %s. Token encontrado: '%s'\n", 
           p->current_token.line, 
           message, 
           p->current_token.lexeme);
    p->has_error = 1;
}

int match(Parser *p, tokenType expected) {
    if (p->current_token.type == expected) {
        advance(p);
        return 1;
    } else {
        char err_msg[100];
        sprintf(err_msg, "Se esperaba %s", token_type_to_str(expected));
        parser_error(p, err_msg);
        return 0;
    }
}

// PROGRAMA -> INSTRUCCION*
void parse_program(Parser *p) {
    printf("\n=== INICIANDO ANALISIS SINTACTICO ===\n");
    while (p->current_token.type != TKN_EOF && !p->has_error) {
        parse_instruction(p);
    }
    if (!p->has_error) {
        printf("Analisis Sintactico completado sin errores.\n");
    } else {
        printf("Analisis Sintactico abortado por errores.\n");
    }
}

// Enrutador de instrucciones
void parse_instruction(Parser *p) {
    switch (p->current_token.type) {
        // 1. Declaracion de variables
        case TKN_INT: case TKN_FLOAT: case TKN_STRING: case TKN_BOOL:
            parse_declaration(p);
            match(p, TKN_SEMICOLON);
            break;
        // 2. Lectura
        case TKN_READ:
            parse_read(p);
            match(p, TKN_SEMICOLON);
            break;
        // 3. Salida
        case TKN_WRITE:
            parse_write(p);
            match(p, TKN_SEMICOLON);
            break;
        // 4. Ciclo FOR
        case TKN_FOR:
            parse_for(p);
            match(p, TKN_SEMICOLON);
            break;
        // 5. Ciclo WHILE
        case TKN_WHILE:
            parse_while(p);
            match(p, TKN_SEMICOLON);
            break;
        // 6. Condicional IF
        case TKN_IF:
            parse_if(p);
            match(p, TKN_SEMICOLON);
            break;
        // 8. Procedimiento
        case TKN_PROC:
            parse_proc(p);
            match(p, TKN_SEMICOLON);
            break;
        // Operaciones unarias o asignaciones que empiezan con un Identificador
        case TKN_IDENTIFIER:
            parse_assignment_or_unary(p);
            match(p, TKN_SEMICOLON);
            break;
        default:
            parser_error(p, "Instruccion no reconocida");
            advance(p);
            break;
    }
}

// DECLARACION -> TIPO_DATO + IDENTIFICADOR + ( ASIGNACION + EXPRESION )?
void parse_declaration(Parser *p) {
    tokenType type = p->current_token.type;
    advance(p); 

    if (p->current_token.type == TKN_IDENTIFIER) {
        install_symbol(p->current_token.lexeme, type);
        advance(p);
    } else {
        parser_error(p, "Se esperaba un nombre de variable");
    }
    
    if (p->current_token.type == TKN_ASSIGN) {
        advance(p);
        parse_expression(p);
    }
}

// IDENTIFICADOR + ( ASIGNACION + EXPRESION | OPERADOR_UNARIO )
void parse_assignment_or_unary(Parser *p) {
    match(p, TKN_IDENTIFIER);
    
    if (p->current_token.type == TKN_ASSIGN) {
        advance(p);
        parse_expression(p);
    } else if (p->current_token.type == TKN_SELF_PLUS || p->current_token.type == TKN_SELF_MINUS) {
        advance(p);
    } else {
        parser_error(p, "Se esperaba '=' o un operador unario '++' / '--'");
    }
}

// LECTURA -> read + IDENTIFICADOR?
void parse_read(Parser *p) {
    match(p, TKN_READ);
    if (p->current_token.type == TKN_IDENTIFIER) {
        advance(p);
    }
}

// SALIDA -> write + EXPRESION
void parse_write(Parser *p) {
    match(p, TKN_WRITE);
    parse_expression(p);
}

// FOR -> for + DECLARACION/ASIGNACION , OPERACION_LOGICA , OPERACION_UNARIA + CUERPO
void parse_for(Parser *p) {
    match(p, TKN_FOR);
    
    // Parte 1: Declaracion o Asignacion
    if (p->current_token.type == TKN_INT || p->current_token.type == TKN_FLOAT || 
        p->current_token.type == TKN_STRING || p->current_token.type == TKN_BOOL) {
        parse_declaration(p);
    } else if (p->current_token.type == TKN_IDENTIFIER) {
        parse_assignment_or_unary(p);
    } else {
        parser_error(p, "Se esperaba declaracion o asignacion inicial en for");
    }
    
    match(p, TKN_COMMA); // ,
    
    // Parte 2: Operacion Logica
    parse_logical_expression(p);
    
    match(p, TKN_COMMA); // ,
    
    // Parte 3: Operacion Unaria
    parse_unary_operation(p);
    
    // Cuerpo
    parse_block(p);
}

// WHILE -> while + OPERACION_LOGICA + CUERPO
void parse_while(Parser *p) {
    match(p, TKN_WHILE);
    parse_logical_expression(p);
    parse_block(p);
}

// IF -> if + OPERACION_LOGICA + CUERPO
void parse_if(Parser *p) {
    match(p, TKN_IF);
    parse_logical_expression(p);
    parse_block(p);
}

// PROC -> proc + IDENTIFICADOR + ( + PARAMETROS* + ) + CUERPO
void parse_proc(Parser *p) {
    match(p, TKN_PROC);
    match(p, TKN_IDENTIFIER);
    match(p, TKN_LPAREN);
    parse_parameters(p);
    match(p, TKN_RPAREN);
    parse_block(p);
}

// PARAMETROS -> (TIPO_DATO + IDENTIFICADOR)
void parse_parameters(Parser *p) {
    if (p->current_token.type == TKN_RPAREN) return;

    do {
        if (p->current_token.type == TKN_INT || p->current_token.type == TKN_FLOAT || 
            p->current_token.type == TKN_STRING || p->current_token.type == TKN_BOOL) {
            advance(p);
            match(p, TKN_IDENTIFIER);
        } else {
            parser_error(p, "Se esperaba un tipo de dato en parametro");
            break;
        }

        if (p->current_token.type == TKN_COMMA) {
            advance(p); 
        } else {
            break;
        }
    } while (1);
}

// CUERPO -> { INSTRUCCION* }
void parse_block(Parser *p) {
    match(p, TKN_LBRACE);
    while (p->current_token.type != TKN_RBRACE && p->current_token.type != TKN_EOF && !p->has_error) {
        parse_instruction(p);
    }
    match(p, TKN_RBRACE);
}

// OPERACION_LOGICA -> EXPRESION + ( > | < | == | != | >= | <= ) + EXPRESION
void parse_logical_expression(Parser *p) {
    parse_expression(p);

    if (p->current_token.type == TKN_GREATER || p->current_token.type == TKN_LESS || 
        p->current_token.type == TKN_GREATER_EQUAL || p->current_token.type == TKN_LESS_EQUAL || 
        p->current_token.type == TKN_EQUAL || p->current_token.type == TKN_NOT_EQUAL) {
        advance(p);
        parse_expression(p);
    } else {
        parser_error(p, "Se esperaba operador relacional");
    }
}

// OPERACION_UNARIA -> (IDENTIFICADOR | VALOR) + ( ++ | -- )
void parse_unary_operation(Parser *p) {
    if (p->current_token.type == TKN_IDENTIFIER || p->current_token.type == TKN_LIT_INT || p->current_token.type == TKN_LIT_FLOAT) {
        advance(p);
    } else {
        parser_error(p, "Se esperaba identificador para operacion unaria");
    }

    if (p->current_token.type == TKN_SELF_PLUS || p->current_token.type == TKN_SELF_MINUS) {
        advance(p);
    } else {
        parser_error(p, "Se esperaba '++' o '--'");
    }
}

// --- JERARQUIA DE EXPRESIONES ARITMETICAS ---

// EXPRESION -> TERMINO ( (+|-) TERMINO )*
void parse_expression(Parser *p) {
    parse_term(p);
    while (p->current_token.type == TKN_PLUS || p->current_token.type == TKN_MINUS) {
        advance(p);
        parse_term(p);
    }
}

// TERMINO -> FACTOR ( (*|/) FACTOR )*
void parse_term(Parser *p) {
    parse_factor(p);
    while (p->current_token.type == TKN_MULT || p->current_token.type == TKN_DIV) {
        advance(p);
        parse_factor(p);
    }
}

// FACTOR -> LITARAL | IDENTIFICADOR | ( EXPRESION )
void parse_factor(Parser *p) {
    if (p->current_token.type == TKN_LIT_INT || 
        p->current_token.type == TKN_LIT_FLOAT || 
        p->current_token.type == TKN_LIT_STRING || 
        p->current_token.type == TKN_LIT_TRUE || 
        p->current_token.type == TKN_LIT_FALSE || 
        p->current_token.type == TKN_IDENTIFIER) {
        install_symbol(p->current_token.lexeme, p->current_token.type);
        advance(p);
    } else if (p->current_token.type == TKN_LPAREN) {
        advance(p);
        parse_expression(p);
        match(p, TKN_RPAREN);
    } else {
        parser_error(p, "Expresion mal formada");
    }
}

void parse(Parser *p) {
    parse_program(p);
}