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
    TKN_LIT_STRING,
    TKN_LIT_TRUE,
    TKN_LIT_FALSE,
    

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
    TKN_SELF_PLUS,
    TKN_SELF_MINUS,

    // Cuerpos y bloques
    TKN_SEMICOLON,
    TKN_LPAREN,
    TKN_RPAREN,
    TKN_LBRACE,
    TKN_RBRACE,
    TKN_COMMA,

    // Estructuras de control y ciclos
    TKN_IF,
    TKN_FOR,
    TKN_WHILE,

    // I/O
    TKN_WRITE,
    TKN_READ,

    // Procedimiento
    TKN_PROC,

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