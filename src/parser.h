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

void parser_create(parser_info_t *parser, args_t args, vector_token_t_t *toks, char *file)
{
    parser->args     = args;
    parser->toks     = toks;
    parser->ast_root = gen_ast_node(NT_TRANSLATION_UNIT, (void*)0);
    parser->cur_node = parser->ast_root;
    parser->pos      = 0;
}

void parser_delete(parser_info_t *parser)
{
    ast_node_delete(parser->ast_root);
}

void parser_decl_parse(parser_info_t *parser);

static vector_error_t_t *err_stk;

void parse(parser_info_t *parser)
{
    for (parser->pos = 0; parser->pos < parser->toks->size; parser->pos++) {
        if (parser->toks->arr[parser->pos].type == TT_TYPE_NAME) {
            parser_decl_parse(parser);
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
    } else {
        exit(76);
    }
}

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
    size_t i = 0;
    while (i < expr->size) {
        if (expr->arr[i].type == TT_STAR) {
            vector_token_t_t *ex1 = vector_token_t_create();
            vector_token_t_t *ex2 = vector_token_t_create();
            parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_BOP, stralc("*")));
            for (size_t j = 0; j < i; j++) {
                vector_token_t_push_back(ex1, expr->arr[j]);
            }
            for (size_t j = i + 1; j < expr->size; j++) {
                vector_token_t_push_back(ex2, expr->arr[j]);
            }

            parser_expr_parse(parser, ex1);
            parser_expr_parse(parser, ex2);

            vector_token_t_free(ex1);
            vector_token_t_free(ex2);
            break;
        }
        i++;
    }

    parser->cur_node = ast_acc;
}

void parser_var_decl_parse(parser_info_t *parser, size_t offset, ctype_t type, token_t ident)
{
    ast_var_info_t *var = (ast_var_info_t*)malloc(sizeof(ast_var_info_t));
    var->type = type;
    strcpy(var->name, ident.val);
    ast_node_add_child(parser->cur_node, gen_ast_node(NT_VARIABLE_DECL, (void*)var));

    if (parser->toks->arr[parser->pos + offset].type == TT_EQ) {
        
    } else
    if (parser->toks->arr[parser->pos = offset].type == TT_SEMICOLON) {
        return;
    } else {
        vector_error_t_push_back(err_stk, gen_error("expected `=`, `;` or `(`",
                                                    parser->args.infile_name,
                                                    parser->toks->arr[parser->pos + 1].line_ref,
                                                    parser->toks->arr[parser->pos + 1].chpos_ref));
    }
}

void parser_decl_parse(parser_info_t *parser)
{
    size_t offset = 1;
    while (parser->toks->arr[parser->pos + offset].type != TT_IDENT) {
        if (parser->pos + ++offset == parser->toks->size) {
            vector_error_t_push_back(err_stk, gen_error("expected identifire",
                                                        parser->args.infile_name,
                                                        parser->toks->arr[parser->pos + 1].line_ref,
                                                        parser->toks->arr[parser->pos + 1].chpos_ref));
            return;
        }
    }
    token_t ident = parser->toks->arr[parser->pos + offset];
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
    size_t i = 1;
    while (parser->toks->arr[parser->pos + offset + i].type != TT_EQ
        && parser->toks->arr[parser->pos + offset + i].type != TT_LPARENT
    ) {
        if (parser->toks->arr[parser->pos + offset + i].type == TT_LSQUARE_BRACKET) {
            if (parser->toks->arr[parser->pos + offset + i + 1].type == TT_RSQUARE_BRACKET) {
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
        i++;
    }
    i = -1;
    while (parser->toks->arr[parser->pos + offset + i].type != TT_TYPE_NAME) {
        if (parser->toks->arr[parser->pos + offset + i].type == TT_STAR) {
            type.modifires[type.modifires_top++] = CTM_POINTER;
        } else
        if (parser->toks->arr[parser->pos + offset + i].type == TT_TYPE_NAME) {
            if (!strcmp(parser->toks->arr[parser->pos + offset + i].val, "const")) {
                type.modifires[type.modifires_top++] = CTM_CONST;
            } else
            if (!strcmp(parser->toks->arr[parser->pos + offset + i].val, "static")) {
                type.modifires[type.modifires_top++] = CTM_STATIC;
            } else
            if (!strcmp(parser->toks->arr[parser->pos + offset + i].val, "unsigned")) {
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

    if (parser->toks->arr[parser->pos + 2].type != TT_LPARENT) {
        // TODO
    }
    parser_var_decl_parse(parser, offset, type, ident);

}

#endif