#ifndef PARSER_STATEMENTS_H
#define PARSER_STATEMENTS_H

#include "common.h"
#include "expr.h"

void parser_stmt_return_parse(parser_info_t *parser, vector_token_t_t *stmt)
{
    ast_node_t *ast_acc = parser->cur_node;
    vector_token_t_t *expr = vector_token_t_create();
    for (size_t i = 1; i < stmt->size; i++) {
        vector_token_t_push_back(expr, stmt->arr[i]);
    }

    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_STMT_RETURN, (void*)0));
    if (expr->size > 1) {
        parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_EXPR, (void*)0));
    }

    parser_expr_parse(parser, expr);
    parser->cur_node = ast_acc;
}

void parser_stmt_asm_parse(parser_info_t *parser, vector_token_t_t *stmt)
{
    ast_node_t *ast_acc = parser->cur_node;
    vector_token_t_t *expr = vector_token_t_create();
    for (size_t i = 2; i < stmt->size - 1; i++) {
        vector_token_t_push_back(expr, stmt->arr[i]);
    }
 
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_STMT_ASM, (void*)0));
    for (size_t i = 0; i < expr->size; i++) {
        ast_node_add_child(parser->cur_node, gen_ast_node(NT_STR_LIT, stralc(expr->arr[i].val)));
    }
    parser->cur_node = ast_acc;
}

void parser_stmt_else_parse(parser_info_t *parser, size_t ns_start, size_t ns_end)
{
    ast_node_t *ast_acc = parser->cur_node;
    
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_STMT_ELSE, (void*)0));

    parser_namespace_parse(parser, ns_start, ns_end);

    parser->cur_node = ast_acc;
}

void parser_stmt_if_parse(parser_info_t *parser, vector_token_t_t *expr, size_t ns_start, size_t ns_end)
{
    ast_node_t *ast_acc = parser->cur_node;
    
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_STMT_IF, (void*)0));
    
    ast_node_t *if_stmt_node = parser->cur_node;
    if (expr->size > 1) {
        parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_EXPR, (void*)0));
    }
    parser_expr_parse(parser, expr);
    parser->cur_node = if_stmt_node;
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_BODY, (void*)0));
    parser_namespace_parse(parser, ns_start, ns_end);
    parser->cur_node = if_stmt_node;

    if (parser->toks->arr[parser->pos + 1].type == TT_KW_ELSE) {
        parser->pos += 3;
        size_t ens_start = parser->pos;
        size_t rb_skip  = 0;
        while (parser->pos < parser->toks->size) {
            if (parser->toks->arr[parser->pos].type == TT_LBRACKET) {
                rb_skip++;
            }
            if (parser->toks->arr[parser->pos].type == TT_RBRACKET) {
                if (!rb_skip) {
                    break;
                }
                rb_skip--;
            }
            parser->pos++;
        }

        parser_stmt_else_parse(parser, ens_start, parser->pos);
    }

    parser->cur_node = ast_acc;
}

#endif