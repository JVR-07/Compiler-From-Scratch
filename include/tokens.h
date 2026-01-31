#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    // Tipos de datos
    TKN_INT, 
    TKN_FLOAT, 
    TKN_STRING, 
    TKN_BOOL,

    // Identificadores y literales
    TKN_IDENTIFIER, 
    TKN_LIT_INT, 
    TKN_LIT_FLOAT,

    // Operadores
    TKN_PLUS, 
    TKN_MINUS,
    TKN_MULT,
    TKN_DIV,
    TKN_ASSIGN,
    TKN_EQUAL,
    TKN_GREATER,
    TKN_GREATER_EQUAL,
    TKN_LESS,
    TKN_LESS_EQUAL,
    TKN_NOT_EQUAL,

    // Cuerpos y bloques
    TKN_SEMICOLON,
    TKN_LPAREN,
    TKN_RPAREN,
    TKN_LBRACE,
    TKN_RBRACE,

    // Estructuras de control y ciclos
    TKN_IF,
    TKN_SWITCH,
    TKN_WHILE,

    // I/O
    TKN_PRINT,
    TKN_READ,

    // Control
    TKN_EOF,
    TKN_ERROR
} tokenType;

typedef struct {
    tokenType type;
    char lexeme[100];
    int line;
} token;

#endif