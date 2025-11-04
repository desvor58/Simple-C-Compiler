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

int parser_parse_bop(parser_info_t *parser, size_t bop_pos, vector_token_t_t *expr)
{
    // printf_s("parsing bop:\n");
    // for (int i = 0; i < expr->size; i++) {
    //     printf_s("    type:%u val:%s", expr->arr[i].type, expr->arr[i].val);
    //     if (i == bop_pos) {
    //         printf_s("  <- bop_pos");
    //     }
    //     putchar('\n');
    // }
    if (bop_pos == 0 || (
        expr->arr[bop_pos - 1].type != TT_IDENT
     && expr->arr[bop_pos - 1].type != TT_INT_LIT
     && expr->arr[bop_pos - 1].type != TT_STR_LIT
     && expr->arr[bop_pos - 1].type != TT_FLOAT_LIT)) {
        return 1;
    }
    if (expr->arr[bop_pos].type == TT_LPARENT) {
        ast_fun_info_t *fun_info = malloc(sizeof(ast_fun_info_t));
        *fun_info = *hashmap_ast_fun_info_t_get(fun_infos, expr->arr[bop_pos - 1].val);
        parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_FUNCTION_CALL, fun_info));
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

        return 0;
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
    return 0;
}

int parser_parse_uop(parser_info_t *parser, size_t uop_pos, vector_token_t_t *expr)
{
    // printf_s("parsing uop:\n");
    // for (int i = 0; i < expr->size; i++) {
    //     printf_s("    type:%u val:%s", expr->arr[i].type, expr->arr[i].val);
    //     if (i == uop_pos) {
    //         printf_s("  <- bop_pos");
    //     }
    //     putchar('\n');
    // }
    vector_token_t_t *ex = vector_token_t_create();
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_UOP, stralc(expr->arr[uop_pos].val)));
    for (size_t j = uop_pos + 1; j < expr->size; j++) {
        vector_token_t_push_back(ex, expr->arr[j]);
    }

    parser_expr_parse(parser, ex);

    vector_token_t_free(ex);
    return 0;
}

typedef enum {
    OT_BOP,
    OT_UOP
} op_type;

typedef struct {
    token_type tt;
    op_type    type;
    int      (*handler)(parser_info_t*, size_t, vector_token_t_t*);
} op_info_t;

static op_info_t op_parse_stack[] = {
    {TT_EQ,        OT_BOP, parser_parse_bop},
    {TT_PLUS,      OT_BOP, parser_parse_bop},
    {TT_MINUS,     OT_BOP, parser_parse_bop},
    {TT_STAR,      OT_BOP, parser_parse_bop},
    {TT_SLASH,     OT_BOP, parser_parse_bop},
    {TT_PLUS,      OT_UOP, parser_parse_uop},
    {TT_MINUS,     OT_UOP, parser_parse_uop},
    {TT_AMPERSAND, OT_UOP, parser_parse_uop},
    {TT_LPARENT,   OT_BOP, parser_parse_bop}
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
    
    size_t op_type_i = 0;
    while (op_type_i < sizeof(op_parse_stack) / sizeof(*op_parse_stack)) {
        size_t i = 0;
        for (;i < expr->size; i++) {
            if (expr->arr[i].type == op_parse_stack[op_type_i].tt) {
                int res = op_parse_stack[op_type_i].handler(parser, i, expr);
                if (res) {
                    continue;
                }
                goto op_parse_end;
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
        }
        op_type_i++;
    }
op_parse_end:

    parser->cur_node = ast_acc;
}

#endif