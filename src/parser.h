#ifndef PARSER_H
#define PARSER_H

#include "types.h"

typedef struct {
    args_t            args;
    vector_token_t_t *toks;
    ast_node_t       *ast_root;
    ast_node_t       *cur_node;
    size_t            pos;
} parser_info_t;

parser_info_t *parser_create(args_t args, vector_token_t_t *toks, char *file)
{
    parser_info_t *parser = malloc(sizeof(parser_info_t));
    parser->args          = args;
    parser->toks          = toks;
    parser->ast_root      = gen_ast_node(NT_TRANSLATION_UNIT, (void*)0);
    parser->cur_node      = parser->ast_root;
    parser->pos           = 0;
    return parser;
}

void parser_delete(parser_info_t *parser)
{
    ast_node_delete(parser->ast_root);
    free(parser);
}

void parser_expr_parse(parser_info_t *parser, vector_token_t_t *expr);
void parser_decl_parse(parser_info_t *parser);

void parser_namespace_parse(parser_info_t *parser, size_t start, size_t end);

static vector_error_t_t *err_stk;

void parse(parser_info_t *parser)
{
    parser_namespace_parse(parser, 0, parser->toks->size);
}

void parser_namespace_parse(parser_info_t *parser, size_t start, size_t end)
{
    for (parser->pos = start; parser->pos < end; parser->pos++) {
        if (parser->toks->arr[parser->pos].type == TT_TYPE_NAME) {
            parser_decl_parse(parser);
        } else {
            vector_token_t_t *expr = vector_token_t_create();
            while (parser->toks->arr[parser->pos].type != TT_SEMICOLON) {
                vector_token_t_push_back(expr, parser->toks->arr[parser->pos]);
                parser->pos++;
            }
            parser_expr_parse(parser, expr);
            vector_token_t_free(expr);
        }
    }
}

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
    TT_SLASH
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
    ast_node_t *ast_acc = parser->cur_node;
    
    size_t bop_type_i = 0;
    while (bop_type_i < 4) {
        size_t i = 0;
        while (i < expr->size) {
            if (expr->arr[i].type == op_parse_stack[bop_type_i]) {
                parser_parse_bop(parser, i, expr);
                goto bop_parse_end;
            }
            i++;
        }
        bop_type_i++;
    }
bop_parse_end:

    parser->cur_node = ast_acc;
}

void parser_var_decl_parse(parser_info_t *parser, size_t ident_offset, size_t se_offset, ctype_t type, token_t ident)
{
    ast_node_t *ast_acc = parser->cur_node;
    ast_var_info_t *var = (ast_var_info_t*)malloc(sizeof(ast_var_info_t));
    var->type = type;
    strcpy(var->name, ident.val);
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_VARIABLE_DECL, (void*)var));

    if (parser->toks->arr[parser->pos + se_offset].type == TT_EQ) {
        vector_token_t_t *expr = vector_token_t_create();
        for (size_t i = se_offset + 1; parser->toks->arr[parser->pos + i].type != TT_SEMICOLON; i++) {
            vector_token_t_push_back(expr, parser->toks->arr[parser->pos + i]);
        }
        if (expr->size == 1) {
            parser_get_val(parser, expr);
        } else {
            parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_EXPR, 0));
            parser_expr_parse(parser, expr);
        }
    } else
    if (parser->toks->arr[parser->pos + se_offset].type == TT_SEMICOLON) {
    } else {
        vector_error_t_push_back(err_stk, gen_error("expected `=`, `;` or `(`",
                                                    parser->args.infile_name,
                                                    parser->toks->arr[parser->pos + 1].line_ref,
                                                    parser->toks->arr[parser->pos + 1].chpos_ref));
    }
    parser->cur_node = ast_acc;
}

void parser_fun_decl_parse(parser_info_t *parser, size_t ident_offset, size_t se_offset, ctype_t type, token_t ident)
{
    ast_node_t *ast_acc = parser->cur_node;

    ast_fun_info_t *fun_info = (ast_fun_info_t*)malloc(sizeof(ast_fun_info_t));
    fun_info->type = type;
    strcpy(fun_info->name, ident.val);
    fun_info->params = list_ast_var_info_t_create();

    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_FUNCTION_DECL, (void*)fun_info));
    
    // for (int i = 1; parser->toks->arr[parser->pos + se_offset + i].type != TT_RPARENT; i++) {
    //     if (parser->toks->arr[parser->pos + se_offset + i].type == TT_TYPE_NAME) {
    //     }
    // }

    if (parser->toks->arr[parser->pos + se_offset + 1].type != TT_RPARENT) {
        puts("EBALABALA KUKAREKU TAM TUDA SUDA ERROR KOROCHE");
    }

    if (parser->toks->arr[parser->pos + se_offset + 2].type == TT_LBRACKET) {
        size_t skip_rb = 0;
        size_t i = 3;
        for (;;i++) {
            if (parser->toks->arr[parser->pos + se_offset + i].type == TT_LBRACKET) {
                skip_rb++;
            }
            if (parser->toks->arr[parser->pos + se_offset + i].type == TT_RBRACKET) {
                if (skip_rb) {
                    skip_rb--;
                    continue;
                }
                break;
            }
        }
        parser_namespace_parse(parser, parser->pos + se_offset + 3, parser->pos + se_offset + i);
    }

    parser->cur_node = ast_acc;
}

void parser_decl_parse(parser_info_t *parser)
{
    size_t ident_offset = 1;
    while (parser->toks->arr[parser->pos + ident_offset].type != TT_IDENT) {
        if (parser->pos + ++ident_offset == parser->toks->size) {
            vector_error_t_push_back(err_stk, gen_error("expected identifire",
                                                        parser->args.infile_name,
                                                        parser->toks->arr[parser->pos + 1].line_ref,
                                                        parser->toks->arr[parser->pos + 1].chpos_ref));
            return;
        }
    }
    token_t ident = parser->toks->arr[parser->pos + ident_offset];
    ctype_t type = {0};
    type.modifires_top = 0;
    if (!strcmp(parser->toks->arr[parser->pos].val, "char")) {
        type.type = CT_CHAR;
    } else
    if (!strcmp(parser->toks->arr[parser->pos].val, "short")) {
        type.type = CT_SHORT;
    } else
    if (!strcmp(parser->toks->arr[parser->pos].val, "int")) {
        type.type = CT_INT;
    } else
    if (!strcmp(parser->toks->arr[parser->pos].val, "long")) {
        type.type = CT_LONG;
    }
    size_t se_offset = ident_offset + 1;
    while (parser->toks->arr[parser->pos + se_offset].type != TT_EQ
        && parser->toks->arr[parser->pos + se_offset].type != TT_LPARENT
    ) {
        if (parser->toks->arr[parser->pos + se_offset].type == TT_LSQUARE_BRACKET) {
            if (parser->toks->arr[parser->pos + se_offset + 1].type == TT_RSQUARE_BRACKET) {
                type.modifires[type.modifires_top++] = CTM_ARRAY;
            } else {
                vector_error_t_push_back(err_stk, gen_error("expected right bracket",
                                                            parser->args.infile_name,
                                                            parser->toks->arr[parser->pos + 1].line_ref,
                                                            parser->toks->arr[parser->pos + 1].chpos_ref));
            }
        } else {
            vector_error_t_push_back(err_stk, gen_error("unknow type modifire",
                                                        parser->args.infile_name,
                                                        parser->toks->arr[parser->pos + 1].line_ref,
                                                        parser->toks->arr[parser->pos + 1].chpos_ref));
        }
        se_offset++;
    }
    size_t i = -1;
    while (parser->toks->arr[parser->pos + ident_offset + i].type != TT_TYPE_NAME) {
        if (parser->toks->arr[parser->pos + ident_offset + i].type == TT_STAR) {
            type.modifires[type.modifires_top++] = CTM_POINTER;
        } else
        if (parser->toks->arr[parser->pos + ident_offset + i].type == TT_TYPE_NAME) {
            if (!strcmp(parser->toks->arr[parser->pos + ident_offset + i].val, "const")) {
                type.modifires[type.modifires_top++] = CTM_CONST;
            } else
            if (!strcmp(parser->toks->arr[parser->pos + ident_offset + i].val, "static")) {
                type.modifires[type.modifires_top++] = CTM_STATIC;
            } else
            if (!strcmp(parser->toks->arr[parser->pos + ident_offset + i].val, "unsigned")) {
                type.modifires[type.modifires_top++] = CTM_UNSIGNED;
            } else {
                vector_error_t_push_back(err_stk, gen_error("unknow type modifire",
                                                            parser->args.infile_name,
                                                            parser->toks->arr[parser->pos + 1].line_ref,
                                                            parser->toks->arr[parser->pos + 1].chpos_ref));
            }
        } else {
            vector_error_t_push_back(err_stk, gen_error("unknow type modifire",
                                                        parser->args.infile_name,
                                                        parser->toks->arr[parser->pos + 1].line_ref,
                                                        parser->toks->arr[parser->pos + 1].chpos_ref));
        }
        i--;
    }

    if (parser->toks->arr[parser->pos + se_offset].type == TT_LPARENT) {
        parser_fun_decl_parse(parser, ident_offset, se_offset, type, ident);
        return;
    }
    parser_var_decl_parse(parser, ident_offset, se_offset, type, ident);
    while (parser->toks->arr[++parser->pos].type != TT_SEMICOLON) {}
}

#endif