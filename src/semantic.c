#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"
#include "symbols.h"
#include "lexer.h"

int semantic_errors = 0;

void semantic_error(int line, const char *message, const char *lexeme) {
    if (lexeme)
        printf("\n\033[1;31mError Semántico\033[0m [Línea %d]: %s '%s'\n", line, message, lexeme);
    else
        printf("\n\033[1;31mError Semántico\033[0m [Línea %d]: %s\n", line, message);
    semantic_errors++;
}

void validate_node(ASTNode *node) {
    if (!node) return;

    // Recorrido Post-fix (hijos primero) para bottom-up eval
    validate_node(node->left);
    validate_node(node->right);
    
    switch (node->type) {
        case NODE_IDENTIFIER: {

            int id = lookup_symbol(node->t.lexeme);

            if (id == -1) {
                semantic_error(node->t.line, "Variable no declarada o fuera de ámbito:", node->t.lexeme);
                node->eval_type = TKN_ERROR;
            } else {
                node->eval_type = get_symbol_type(id);
            }
            break;
        }

        case NODE_LITERAL: {

            if (node->t.type == TKN_LIT_INT) node->eval_type = TKN_INT;
            else if (node->t.type == TKN_LIT_FLOAT) node->eval_type = TKN_FLOAT;
            else if (node->t.type == TKN_LIT_STRING) node->eval_type = TKN_STRING;
            else if (node->t.type == TKN_LIT_TRUE || node->t.type == TKN_LIT_FALSE) node->eval_type = TKN_BOOL;
            
            break;
        }

        case NODE_UNARY_OP: {
            
            tokenType operand_type = node->left ? node->left->eval_type : TKN_EOF;

            if (node->t.type == TKN_NOT) {
                
                if (operand_type != TKN_BOOL && operand_type != TKN_ERROR) {
                    semantic_error(node->t.line, "Operación unaria lógica ! sbore tipo no booleano", node->t.lexeme);
                    node->eval_type = TKN_ERROR;
                } else {
                    node->eval_type = TKN_BOOL;
                }
            }
            else if (node->t.type == TKN_SELF_PLUS || node->t.type == TKN_SELF_MINUS) {

                if (operand_type != TKN_INT && operand_type != TKN_FLOAT && operand_type != TKN_ERROR) {
                    semantic_error(node->t.line, "Operación unaria iterativa ++/-- sobre tipo no numérico.", node->t.lexeme);
                    node->eval_type = TKN_ERROR;
                } else {
                    node->eval_type = operand_type;
                }
                break;
            }
        }

        case NODE_BINARY_OP: {

            tokenType leftType = node->left ? node->left->eval_type : TKN_EOF;
            tokenType rightType = node->right ? node->right->eval_type : TKN_EOF;

            // si los hijos fallaron
            if (leftType == TKN_ERROR || rightType == TKN_ERROR) {
                
                node->eval_type = TKN_ERROR;
                break;
            }

            if (node->t.type == TKN_AND || node->t.type == TKN_OR) {

                if(leftType == TKN_BOOL && rightType == TKN_BOOL) {
                    
                    node->eval_type = TKN_BOOL;

                } else {
                    
                    semantic_error(node->t.line, "Tipos incompatibles en operación lógica:", node->t.lexeme);
                    node->eval_type = TKN_ERROR;
                }
            }
            else if (node->t.type == TKN_PLUS || node->t.type == TKN_MINUS || node->t.type == TKN_MULT || node->t.type == TKN_DIV || node->t.type == TKN_POWER) {
                
                if (leftType == TKN_INT && rightType == TKN_INT) {
                
                    node->eval_type = TKN_INT;
                
                } else if ((leftType == TKN_FLOAT || leftType == TKN_INT) && 
                           (rightType == TKN_FLOAT || rightType == TKN_INT)) {
                
                    node->eval_type = TKN_FLOAT;
                
                } else if (node->t.type == TKN_PLUS && leftType == TKN_STRING && rightType == TKN_STRING) {
                
                    node->eval_type = TKN_STRING; 
                
                } else {
                    
                    semantic_error(node->t.line, "Tipos incompatibles en operación aritmética:", node->t.lexeme);
                    node->eval_type = TKN_ERROR;
                }
            } else if (node->t.type == TKN_GREATER || node->t.type == TKN_LESS || 
                       node->t.type == TKN_GREATER_EQUAL || node->t.type == TKN_LESS_EQUAL || 
                       node->t.type == TKN_EQUAL || node->t.type == TKN_NOT_EQUAL) {
            
                node->eval_type = TKN_BOOL;
            }

            break;
        }

        case NODE_ASSIGN: {
            tokenType target_type = node->left ? node->left->eval_type : TKN_EOF;
            tokenType expr_type = node->right ? node->right->eval_type : TKN_EOF;
            
            if (target_type == TKN_ERROR || expr_type == TKN_ERROR) {
                
                node->eval_type = TKN_ERROR;
                break;
            }

            if (target_type != expr_type) {
                
                if (!(target_type == TKN_FLOAT && expr_type == TKN_INT)) {
                    
                    semantic_error(node->t.line, "Tipos incompatibles en la asignación.", NULL);
                    node->eval_type = TKN_ERROR;
                }
            }
            break;
        }

        default:
            break;
    }
}

void analyze_semantic(ASTNode *root) {
    if (!root) return;
    
    semantic_errors = 0;
    
    validate_node(root);
}
