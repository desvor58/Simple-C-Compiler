#ifndef PARSER_EXPR_H
#define PARSER_EXPR_H

#include "common.h"

void parser_get_val(parser_info_t *parser, vector_token_t_t *expr)
{
    // puts("in num!");
    if (expr->arr[0].type == TT_INT_LIT) {
        ast_node_add_child(parser->cur_node, gen_ast_node(NT_INT_LIT, stralc(expr->arr[0].val)));
    } else
    if (expr->arr[0].type == TT_STR_LIT) {
        ast_node_add_child(parser->cur_node, gen_ast_node(NT_STR_LIT, stralc(expr->arr[0].val)));
    } else
    if (expr->arr[0].type == TT_IDENT) {
        ast_node_add_child(parser->cur_node, gen_ast_node(NT_IDENT, stralc(expr->arr[0].val)));
    } else {
        exit(76);
    }
}

void parser_parse_bop(parser_info_t *parser, size_t bop_pos, vector_token_t_t *expr)
{
    // printf_s("parsing bop:\n");
    // for (int i = 0; i < expr->size; i++) {
    //     printf_s("    type:%u val:%s\n", expr->arr[i].type, expr->arr[i].val);
    // }
    if (expr->arr[bop_pos].type == TT_LPARENT) {
        parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_FUNCTION_CALL, (void*)0));
        ast_node_add_child(parser->cur_node, gen_ast_node(NT_IDENT, stralc(expr->arr[bop_pos - 1].val)));
        
        size_t i = bop_pos + 1;
        for (;;) {
            if (expr->arr[i].type == TT_RPARENT) {
                break;
            }
            vector_token_t_t *arg_expr = vector_token_t_create();
            if (expr->arr[i].type == TT_COMA) {
                i++;
            }
            size_t skip_rparent = 0;
            while (expr->arr[i].type != TT_COMA) {
                if (expr->arr[i].type == TT_RPARENT) {
                    if (skip_rparent) {
                        skip_rparent--;
                    } else {
                        break;
                    }
                }
                if (expr->arr[i].type == TT_LPARENT) {
                    skip_rparent++;
                }
                vector_token_t_push_back(arg_expr, expr->arr[i]);
                i++;
            }
            ast_node_t *acc = parser->cur_node;
            if (arg_expr->size > 1) {
                parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_EXPR, (void*)0));
            }
            parser_expr_parse(parser, arg_expr);
            parser->cur_node = acc;
            vector_token_t_free(arg_expr);
        }

        return;
    }
    vector_token_t_t *ex1 = vector_token_t_create();
    vector_token_t_t *ex2 = vector_token_t_create();
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_BOP, stralc(expr->arr[bop_pos].val)));
    for (size_t j = 0; j < bop_pos; j++) {
        vector_token_t_push_back(ex1, expr->arr[j]);
    }
    for (size_t j = bop_pos + 1; j < expr->size; j++) {
        vector_token_t_push_back(ex2, expr->arr[j]);
    }

    parser_expr_parse(parser, ex1);
    parser_expr_parse(parser, ex2);

    vector_token_t_free(ex1);
    vector_token_t_free(ex2);
}

static token_type op_parse_stack[] = {
    TT_EQ,
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_LPARENT
};

void parser_expr_parse(parser_info_t *parser, vector_token_t_t *expr)
{
    // printf_s("parsing this:\n");
    // for (int i = 0; i < expr->size; i++) {
    //     printf_s("    type:%u val:%s\n", expr->arr[i].type, expr->arr[i].val);
    // }
    
    if (expr->size == 1) {
        parser_get_val(parser, expr);
        return;
    }
    if (expr->arr[0].type == TT_LPARENT) {
        size_t pi = 1;
        size_t skip_parent = 0;
        for (;;) {
            if (expr->arr[pi].type == TT_LPARENT) {
                skip_parent++;
            }
            if (expr->arr[pi].type == TT_RPARENT) {
                if (skip_parent) {
                    skip_parent--;
                } else {
                    break;
                }
            }
            pi++;
        }
        if (pi == expr->size - 1) {
            vector_token_t_t *subparent_expr = vector_token_t_create();
            for (size_t i = 1; i < expr->size - 1; i++) {
                vector_token_t_push_back(subparent_expr, expr->arr[i]);
            }
            parser_expr_parse(parser, subparent_expr);
            vector_token_t_free(subparent_expr);
            return;
        }
    }
    ast_node_t *ast_acc = parser->cur_node;
    
    size_t bop_type_i = 0;
    while (bop_type_i < sizeof(op_parse_stack) / sizeof(*op_parse_stack)) {
        size_t i = 0;
        while (i < expr->size) {
            if (expr->arr[i].type == op_parse_stack[bop_type_i]) {
                parser_parse_bop(parser, i, expr);
                goto bop_parse_end;
            }
            if (expr->arr[i].type == TT_LPARENT) {
                i++;
                size_t skip_parent = 0;
                for (;;) {
                    if (expr->arr[i].type == TT_LPARENT) {
                        skip_parent++;
                    }
                    if (expr->arr[i].type == TT_RPARENT) {
                        if (skip_parent) {
                            skip_parent--;
                        } else {
                            break;
                        }
                    }
                    i++;
                }
lparent_parse_end:
            }
            i++;
        }
        bop_type_i++;
    }
bop_parse_end:

    parser->cur_node = ast_acc;
}

#endif