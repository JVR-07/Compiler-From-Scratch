#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "symbols.h"
#include "ast.h"
#include "semantic.h"
#include "codegen.h"

void init_parser(Parser *p, Lexer *l) {
    p->lexer = l;
    p->has_error = 0;
    p->is_global_scope = 1;
    advance(p);
}

void advance(Parser *p) {
    p->current_token = next_token(p->lexer);
}

void parser_error(Parser *p, const char *message) {
    
    if (p->has_error) return;

    if (p->current_token.type == TKN_ERROR) {
        printf("\n\033[1;31mError Léxico\033[0m [Línea %d]: Carácter no reconocido '%s'\n", 
               p->current_token.line, p->current_token.lexeme);
    } else {
        printf("\n\033[1;31mError Sintáctico\033[0m [Línea %d]: %s. Cerca de: '%s'\n", 
               p->current_token.line, message, p->current_token.lexeme);
    }
    p->has_error = 1;
}

void synchronize(Parser *p) {
    p->has_error = 0;

    while (p->current_token.type != TKN_EOF) {
        if (p->current_token.type == TKN_SEMICOLON) {
            advance(p);
            p->has_error = 0;
            return;
        }

        switch (p->current_token.type) {
            case TKN_INT:
            case TKN_FLOAT:
            case TKN_STRING:
            case TKN_BOOL:
            case TKN_IF:
            case TKN_FOR:
            case TKN_WHILE:
            case TKN_PROC:
            case TKN_WRITE:
            case TKN_READ:
            case TKN_ELSE:
            case TKN_RBRACE:
                return;

            default:
                advance(p); 
                break;
        }
    }
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
    while (p->current_token.type != TKN_EOF) {
        parse_instruction(p);

        if(p->has_error) {
            synchronize(p);
        }
    }
    
    if (p->has_error) {
        printf("\n\033[1;33mAviso\033[0m: Analisis Sintactico abortado por errores.\n");
    }
}

void process_expression(ASTNode *node, Parser *p) {
    if (!node) return;

    // printf("\n--> Arbol de la Expresión:\n");  // COMENTADO: solo mostrar ensamblador
    // print_ast(node, 0, "ROOT");                 // COMENTADO: solo mostrar ensamblador

    if (!p->has_error) {
        analyze_semantic(node);
        if (semantic_errors == 0) {
            gen_expr(node);
        }
    } else {
        // printf("\n\033[1;33mAviso\033[0m: Análisis Semántico omitido.\n"); // COMENTADO
    }
    
    free_ast(node);
}

void parse_instruction(Parser *p) {
    switch (p->current_token.type) {
        case TKN_INT: case TKN_FLOAT: case TKN_STRING: case TKN_BOOL:
            parse_declaration(p);
            match(p, TKN_SEMICOLON);
            break;
        case TKN_READ:
            parse_read(p);
            match(p, TKN_SEMICOLON);
            break;
        case TKN_WRITE:
            parse_write(p);
            match(p, TKN_SEMICOLON);
            break;
        case TKN_FOR:
            parse_for(p);
            match(p, TKN_SEMICOLON);
            break;
        case TKN_WHILE:
            parse_while(p);
            match(p, TKN_SEMICOLON);
            break;
        case TKN_IF:
            parse_if(p);
            match(p, TKN_SEMICOLON);
            break;
        case TKN_PROC:
            if (!p->is_global_scope) {
                parser_error(p, "'proc' solo puede declararse en el scope global");
                while (p->current_token.type != TKN_SEMICOLON && p->current_token.type != TKN_EOF) {
                    advance(p);
                }
            } else {
                parse_proc(p);
                match(p, TKN_SEMICOLON);
            }
            break;
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

void parse_declaration(Parser *p) {
    token type_tkn = p->current_token;
    advance(p); 

    token id_tkn;
    if (p->current_token.type == TKN_IDENTIFIER) {
        id_tkn = p->current_token;
        install_symbol(id_tkn.lexeme, TKN_IDENTIFIER, type_tkn.type);
        alloc_var(id_tkn.lexeme, type_tkn.type);
        advance(p);
    } else {
        parser_error(p, "Se esperaba un nombre de variable");
        return;
    }
    
    if (p->current_token.type == TKN_ASSIGN) {
        token assign_tkn = p->current_token;
        advance(p);
        
        ASTNode *assign = create_node(NODE_ASSIGN, assign_tkn);
        assign->left = create_node(NODE_IDENTIFIER, id_tkn);
        assign->right = parse_logical_or(p);
        
        process_expression(assign, p);
    }
}

void parse_assignment_or_unary(Parser *p) {
    token id_tkn = p->current_token;
    ASTNode *id_node = create_node(NODE_IDENTIFIER, id_tkn);
    match(p, TKN_IDENTIFIER);
    
    if (p->current_token.type == TKN_ASSIGN) {
        token assign_tkn = p->current_token;
        ASTNode *assign = create_node(NODE_ASSIGN, assign_tkn);
        assign->left = id_node;
        advance(p);
        assign->right = parse_logical_or(p);
        process_expression(assign, p);
    } else if (p->current_token.type == TKN_SELF_PLUS || p->current_token.type == TKN_SELF_MINUS) {
        token unary_tkn = p->current_token;
        ASTNode *unary = create_node(NODE_UNARY_OP, unary_tkn);
        unary->left = id_node;
        advance(p);
        process_expression(unary, p);
    } else {
        parser_error(p, "Se esperaba '=' o un operador unario '++' / '--'");
        free_ast(id_node);
    }
}

void parse_read(Parser *p) {
    token read_tkn = p->current_token;
    match(p, TKN_READ);
    
    if(p->current_token.type == TKN_IDENTIFIER) {
        ASTNode *id_node = create_node(NODE_IDENTIFIER, p->current_token);
        ASTNode *read_node = create_node(NODE_READ, read_tkn);
        read_node->left = id_node;
    
        advance(p);

        process_expression(read_node, p);
    } else if (p->current_token.type != TKN_SEMICOLON) {
        parser_error(p, "Se esperaba un identificador o ';' después de 'read'");
    }
}

void parse_write(Parser *p) {
    token write_tkn = p->current_token;
    match(p, TKN_WRITE);

    if (p->current_token.type == TKN_SEMICOLON) {
        parser_error(p, "Se esperaba un argumento");
        return;
    }

    ASTNode *expr = parse_logical_or(p);

    if(expr) {
        ASTNode *write_node = create_node(NODE_WRITE, write_tkn);
        write_node->left = expr;
        process_expression(write_node, p);
    }
}

void parse_for(Parser *p) {
    match(p, TKN_FOR);
    enter_scope();
    
    if (p->current_token.type == TKN_INT || p->current_token.type == TKN_FLOAT || 
        p->current_token.type == TKN_STRING || p->current_token.type == TKN_BOOL) {
        parse_declaration(p);
    } else if (p->current_token.type == TKN_IDENTIFIER) {
        parse_assignment_or_unary(p);
    } else {
        parser_error(p, "Se esperaba declaracion o asignacion inicial en for");
    }
    
    match(p, TKN_COMMA);
    ASTNode *cond = parse_logical_or(p);
    process_expression(cond, p);
    
    match(p, TKN_COMMA);
    ASTNode *inc = parse_unary_operation(p);
    process_expression(inc, p);
    
    parse_block(p);
    
    exit_scope();
}

void parse_while(Parser *p) {
    match(p, TKN_WHILE);

    if(p->current_token.type == TKN_LBRACE) {
        parser_error(p, "Se esperaba una comparación");
    } else {
        ASTNode *cond = parse_logical_or(p);
        process_expression(cond, p);
    }

    parse_block(p);
}

void parse_if(Parser *p) {
    match(p, TKN_IF);

    if(p->current_token.type == TKN_LBRACE) {
        parser_error(p, "Se esperaba una comparación");
    } else {
        ASTNode *cond = parse_logical_or(p);
        process_expression(cond, p);
    }

    parse_block(p);
}

void parse_proc(Parser *p) {
    match(p, TKN_PROC);

    if (p->current_token.type == TKN_IDENTIFIER) {
        install_symbol(p->current_token.lexeme, TKN_IDENTIFIER, TKN_PROC);
        match(p, TKN_IDENTIFIER);
    }
    
    enter_scope();

    match(p, TKN_LPAREN);
    parse_parameters(p);

    if (p->has_error) {
        while (p->current_token.type != TKN_LBRACE && p->current_token.type != TKN_EOF) {
            advance(p);
        }
    } else {
        match(p, TKN_RPAREN);
    }
    
    parse_block(p);
    
    exit_scope();
}

void parse_parameters(Parser *p) {
    if (p->current_token.type == TKN_RPAREN) return;

    while (p->current_token.type != TKN_RPAREN && p->current_token.type != TKN_EOF) {
        
        if (p->current_token.type == TKN_INT || p->current_token.type == TKN_FLOAT || 
            p->current_token.type == TKN_STRING || p->current_token.type == TKN_BOOL) {
            
            token type_tkn = p->current_token;
            advance(p);
            
            if (p->current_token.type == TKN_IDENTIFIER) {
                install_symbol(p->current_token.lexeme, TKN_IDENTIFIER, type_tkn.type);
            }
            match(p, TKN_IDENTIFIER);

        } else {
            parser_error(p, "Se esperaba un tipo de dato");
            advance(p);
        }

        if (p->current_token.type == TKN_COMMA) {
            advance(p); 

            if (p->current_token.type == TKN_RPAREN) {
                parser_error(p, "Se esperaba un parámetro después de la coma");
                break; 
            }
        } else if (p->current_token.type != TKN_RPAREN) {
            parser_error(p, "Se esperaba ',' o ')'");
            break;
        }
    }
}

void parse_block(Parser *p) {
    match(p, TKN_LBRACE);
    
    enter_scope();

    int prev_global = p->is_global_scope;
    p->is_global_scope = 0;

    while (p->current_token.type != TKN_RBRACE && p->current_token.type != TKN_EOF && !p->has_error) {
        
        parse_instruction(p);
        
        if(p->has_error) {
            synchronize(p);
        }
    }

    p->is_global_scope = prev_global;

    exit_scope();
    
    match(p, TKN_RBRACE);
}

ASTNode* parse_logical_or(Parser *p) {
    ASTNode *node = parse_logical_and(p);

    while(p->current_token.type == TKN_OR) {
        token op = p->current_token;
        advance(p);
        node = create_binary_node(op, node, parse_logical_and(p));
    }
    return node;
}

ASTNode* parse_logical_and(Parser *p) {
    ASTNode *node = parse_relational(p);

    while(p->current_token.type == TKN_AND) {
        token op = p->current_token;
        advance(p);
        node = create_binary_node(op, node, parse_relational(p));
    }
    return node;
}

ASTNode* parse_relational(Parser *p) {
    ASTNode *node1 = parse_expression(p);
    
    if (p->current_token.type == TKN_GREATER || p->current_token.type == TKN_LESS ||
        p->current_token.type == TKN_GREATER_EQUAL || p->current_token.type == TKN_LESS_EQUAL ||
        p->current_token.type == TKN_EQUAL || p->current_token.type == TKN_NOT_EQUAL) {
        
        token op_tkn = p->current_token;
        advance(p);
        ASTNode *node2 = parse_expression(p);
        return create_binary_node(op_tkn, node1, node2);
    }
    return node1;
}

ASTNode* parse_logical_expression(Parser *p) {
    ASTNode *node1 = parse_expression(p);
    
    if (p->current_token.type == TKN_GREATER || p->current_token.type == TKN_LESS || 
        p->current_token.type == TKN_GREATER_EQUAL || p->current_token.type == TKN_LESS_EQUAL || 
        p->current_token.type == TKN_EQUAL || p->current_token.type == TKN_NOT_EQUAL) {
        
        token op_tkn = p->current_token;
        advance(p);
        ASTNode *node2 = parse_expression(p);
        return create_binary_node(op_tkn, node1, node2);
    }
    parser_error(p, "Se esperaba una comparación");
    return node1;
}

ASTNode* parse_unary_operation(Parser *p) {
    ASTNode *operand = NULL;
    if (p->current_token.type == TKN_IDENTIFIER || p->current_token.type == TKN_LIT_INT || p->current_token.type == TKN_LIT_FLOAT) {
        if(p->current_token.type == TKN_IDENTIFIER) {
            operand = create_node(NODE_IDENTIFIER, p->current_token);
        } else {
            operand = create_node(NODE_LITERAL, p->current_token);
        }
        advance(p);
    } else {
        parser_error(p, "Se esperaba identificador para operacion unaria");
    }

    if (p->current_token.type == TKN_SELF_PLUS || p->current_token.type == TKN_SELF_MINUS) {
        token op_tkn = p->current_token;
        ASTNode *unary = create_node(NODE_UNARY_OP, op_tkn);
        advance(p);
        unary->left = operand;
        return unary;
    } else {
        parser_error(p, "Se esperaba '++' o '--'");
        return operand;
    }
}

// --- JERARQUIA DE EXPRESIONES ARITMETICAS ---

ASTNode* parse_expression(Parser *p) {
    ASTNode *node = parse_term(p);

    while (p->current_token.type == TKN_PLUS || p->current_token.type == TKN_MINUS) {
       token op = p->current_token;
        advance(p);
        
        node = create_binary_node(op, node, parse_term(p));
    }
    return node;
}

ASTNode* parse_term(Parser *p) {
    ASTNode* node = parse_power(p);

    while (p->current_token.type == TKN_MULT || p->current_token.type == TKN_DIV) {
        token op = p->current_token;
        advance(p);
        node = create_binary_node(op, node, parse_power(p));
    }
    return node;
}

ASTNode* parse_power(Parser *p) {
    ASTNode* node = parse_factor(p);

    while (p->current_token.type == TKN_POWER) {
        token op = p->current_token;
        advance(p);
        node = create_binary_node(op, node, parse_factor(p));
    }
    return node;
}

ASTNode* parse_factor(Parser *p) {
    token t = p->current_token;

    if (t.type == TKN_NOT) {
        advance(p);
        ASTNode *operand = parse_factor(p);
        ASTNode *not_node = create_node(NODE_UNARY_OP, t);
        not_node->left = operand;
        return not_node;
    }

    if (t.type == TKN_LIT_INT || 
        t.type == TKN_LIT_FLOAT || 
        t.type == TKN_LIT_STRING || 
        t.type == TKN_LIT_TRUE || 
        t.type == TKN_LIT_FALSE || 
        t.type == TKN_IDENTIFIER) {
        
        NodeType node_type = (t.type == TKN_IDENTIFIER) ? NODE_IDENTIFIER : NODE_LITERAL;
        
        if (t.type != TKN_IDENTIFIER) {
            install_symbol(t.lexeme, t.type, t.type);
        }
        advance(p);
        return create_node(node_type, t);
    } else if (t.type == TKN_LPAREN) {
        advance(p);
        ASTNode *node = parse_logical_or(p);
        match(p, TKN_RPAREN);
        return node;
    }

    parser_error(p, "Expresión mal formada o factor no esperado");
    return NULL;
}