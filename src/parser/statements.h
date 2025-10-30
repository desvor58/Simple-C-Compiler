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

#endif