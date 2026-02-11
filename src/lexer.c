#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

void init_lexer(Lexer *l, FILE *f) {
    l->source = f;
    l->line_buf = NULL;
    l->line_cap = 0;
    l->line_pos = 0;
    l->line_num = 0;
}

void close_lexer(Lexer *l) {
    if (l->line_buf) free(l->line_buf); 
}

static int get_char(Lexer *l) {
    if (l->line_buf == NULL || l->line_buf[l->line_pos] == '\0') {
        ssize_t read = getline(&l->line_buf, &l->line_cap, l->source);
        if (read == -1) return EOF;
        l->line_pos = 0;
        l->line_num++;
    }
    return l->line_buf[l->line_pos++];
}

static void unget_char(Lexer *l) {
    if (l->line_pos > 0) l->line_pos--;
}

token next_token(Lexer *l) {
    // Iniciar estructura de token
    token t;
    t.lexeme[0] = '\0';
    t.line = l->line_num;

    // Inicializar c con el primer caracter del input
    int c = get_char(l);

    while (isspace(c)) {
        c = get_char(l);
    }

    if (c == EOF) {
        t.type = TKN_EOF;
        return t;
    }

    t.line = l->line_num;

    // cadenas
    if (c == '"') {
        int i = 0;
        t.lexeme[i++] = c; 
        
        while ((c = get_char(l)) != '"' && c != EOF) {
            if (i < 99) t.lexeme[i++] = c;
            if (c == '\n') l->line_num++; 
        }

        if (c == '"') {
            if (i < 99) t.lexeme[i++] = c;
            t.lexeme[i] = '\0';
            t.type = TKN_LIT_STRING;
        } else {
            t.type = TKN_ERROR;
        }
        return t;
    }

    // digitos
    if (isdigit(c) || c == '-') {
        int i = 0;
        int is_negative = (c == '-');

        if (is_negative) {
            int next = get_char(l);
            if (isdigit(next)) {
                t.lexeme[i++] = c; 
                c = next;          
            } else {
                unget_char(l);
                goto handle_operators;
            }
        }

        t.lexeme[i++] = c;
        while (isdigit(c = get_char(l))) {
            if (i < 99) t.lexeme[i++] = c;
        }
        if (c == '.') {
            t.lexeme[i++] = c;
            while (isdigit(c = get_char(l))) {
                if (i < 99) t.lexeme[i++] = c;
            }
            t.type = TKN_LIT_FLOAT;
        } else {
            t.type = TKN_LIT_INT;
        }
        
        t.lexeme[i] = '\0';
        unget_char(l);
        return t;
    }

    // palabras reservadas e identificadores
    if (isalpha(c) || c == '_') {
        int i = 0;
        t.lexeme[i++] = c;
        while (isalnum(c = get_char(l)) || c == '_') {
            if (i < 99) t.lexeme[i++] = c;
        }
        t.lexeme[i] = '\0';
        unget_char(l);

        if (strcmp(t.lexeme, "int") == 0) t.type = TKN_INT;
        else if (strcmp(t.lexeme, "float") == 0) t.type = TKN_FLOAT;
        else if (strcmp(t.lexeme, "string") == 0) t.type = TKN_STRING;
        else if (strcmp(t.lexeme, "bool") == 0) t.type = TKN_BOOL;
        else if (strcmp(t.lexeme, "true") == 0) t.type = TKN_LIT_TRUE; 
        else if (strcmp(t.lexeme, "false") == 0) t.type = TKN_LIT_FALSE;
        else if (strcmp(t.lexeme, "if") == 0) t.type = TKN_IF;
        else if (strcmp(t.lexeme, "write") == 0) t.type = TKN_WRITE;
        else if (strcmp(t.lexeme, "read") == 0) t.type = TKN_READ;
        else if (strcmp(t.lexeme, "while") == 0) t.type = TKN_WHILE;
        else if (strcmp(t.lexeme, "for") == 0) t.type = TKN_FOR;
        else if (strcmp(t.lexeme, "proc") == 0) t.type = TKN_PROC;
        else t.type = TKN_IDENTIFIER;

        return t;
    }

handle_operators:
    // operadores
    t.lexeme[0] = c; t.lexeme[1] = '\0';

    switch (c) {
        case '+': 
            c = get_char(l);
            if(c == '+') {
                t.type = TKN_SELF_PLUS;
                strcat(t.lexeme, "+");
            } else {
                unget_char(l);
                t.type = TKN_PLUS;
            }
            break;

        case '-': 
            c = get_char(l);
            if(c == '-') {
                t.type = TKN_SELF_MINUS;
                strcat(t.lexeme, "-");
            } else {
                unget_char(l);
                t.type = TKN_MINUS;
            }
            break;

        case '*': t.type = TKN_MULT; break;
        case '/': t.type = TKN_DIV; break;
        case ';': t.type = TKN_SEMICOLON; break;
        case '(': t.type = TKN_LPAREN; break;
        case ')': t.type = TKN_RPAREN; break;
        case '{': t.type = TKN_LBRACE; break;
        case '}': t.type = TKN_RBRACE; break;
        case '<': t.type = TKN_LESS; break;
        case '>': t.type = TKN_GREATER; break;
        case ',': t.type = TKN_COMMA; break;
        
        case '=': 
            c = get_char(l);
            if (c == '=') {
                t.type = TKN_EQUAL;
                strcat(t.lexeme, "=");
            } else if (c == '<') {
                t.type = TKN_LESS_EQUAL;
                strcat(t.lexeme, "<");
            } else if (c == '>') {
                t.type = TKN_GREATER_EQUAL;
                strcat(t.lexeme, ">");
            } else {
                unget_char(l);
                t.type = TKN_ASSIGN;
            }
            break;

        case '!':
            c = get_char(l);
            if (c == '=') {
                t.type = TKN_NOT_EQUAL;
                strcat(t.lexeme, "=");
            } else {
                unget_char(l);
                t.type = TKN_ERROR;
            }
            break;

        default:
            t.type = TKN_ERROR;
            break;
    }

    return t;
}

// funcion auxiliar para imprimir tipo de dato con formato
const char* token_type_to_str(tokenType t) {
    switch(t) {
        // tipos datos
        case TKN_INT: return "INT"; 
        case TKN_FLOAT: return "FLOAT"; 
        case TKN_STRING: return "STRING"; 
        case TKN_BOOL: return "BOOL";
        // Identificadores y literales
        case TKN_IDENTIFIER: return "IDENTIFIER"; 
        case TKN_LIT_STRING: return "LITERAL_STRING";
        case TKN_LIT_INT: return "LITERAL_INT";
        case TKN_LIT_FLOAT: return "LITERAL_FLOAT";
        case TKN_LIT_TRUE: return "LITERAL_TRUE";
        case TKN_LIT_FALSE: return "LITERAL_FALSE";
        // Operadores
        case TKN_PLUS: return "PLUS";
        case TKN_SELF_PLUS: return "SELF_PLUS";
        case TKN_MINUS: return "MINUS";
        case TKN_SELF_MINUS: return "SELF_MINUS";
        case TKN_MULT: return "MULTI";
        case TKN_DIV: return "DIVISION";
        case TKN_ASSIGN: return "ASSIGN";
        case TKN_EQUAL: return "EQUAL";
        case TKN_GREATER: return "GREATER";
        case TKN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TKN_LESS: return "LESS";
        case TKN_LESS_EQUAL: return "LESS_EQUAL";
        case TKN_NOT_EQUAL: return "NOT_EQUAL";
        // Cuerpos y bloques
        case TKN_SEMICOLON: return "SEMICOLON";
        case TKN_LPAREN: return "LEFT_PAREN";
        case TKN_RPAREN: return "RIGHT_PAREN";
        case TKN_LBRACE: return "LEFT_BRACE";
        case TKN_RBRACE: return "RIGHT_BRACE";
        case TKN_COMMA: return "COMMA";
        // Estructuras de control y ciclos
        case TKN_IF: return "IF";
        case TKN_WHILE: return "WHILE";
        case TKN_FOR: return "FOR";
        case TKN_PROC: return "PROC";
        // I/O
        case TKN_WRITE: return "WRITE";
        case TKN_READ: return "READ";

        // Control
        case TKN_EOF: return "EOF";
        case TKN_ERROR: return "ERROR";
        
        // generico
        default: return "TOKEN";
    }
}