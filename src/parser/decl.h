#ifndef PARSER_DECL_H
#define PARSER_DECL_H

#include "common.h"
#include "expr.h"
#include "statements.h"

ctype_t parser_type_parse(parser_info_t *parser, vector_token_t_t *type_slice, size_t ident_offset)
{
    ctype_t type = {0};
    type.modifires_top = 0;
    if (type_slice->arr[0].type != TT_TYPE_NAME) {
        vector_error_t_push_back(parser->err_stk, gen_error("Expected type name",
                                                    parser->args.infile_name,
                                                    type_slice->arr[0].line_ref,
                                                    type_slice->arr[0].chpos_ref));
        return (ctype_t){0};
    }

    size_t i = ident_offset + 1;
    while (i < type_slice->size) {
        if (type_slice->arr[i].type == TT_LSQUARE_BRACKET
         && type_slice->arr[i + 1].type == TT_RSQUARE_BRACKET) {
            type.modifires[type.modifires_top++] = CTM_ARRAY;
        }
        i++;
    }

    i = 0;
    while (i < ident_offset) {
        if (!strcmp(type_slice->arr[i].val, "unsigned")) {
            type.modifires[type.modifires_top++] = CTM_UNSIGNED;
        } else
        if (!strcmp(type_slice->arr[i].val, "static")) {
            type.modifires[type.modifires_top++] = CTM_STATIC;
        } else
        if (!strcmp(type_slice->arr[i].val, "const")) {
            type.modifires[type.modifires_top++] = CTM_CONST;
        } else
        if (!strcmp(type_slice->arr[i].val, "*")) {
            type.modifires[type.modifires_top++] = CTM_POINTER;
        } else {
            if (!strcmp(type_slice->arr[i].val, "char")) {
                type.type = CT_CHAR;
            } else
            if (!strcmp(type_slice->arr[i].val, "short")) {
                type.type = CT_SHORT;
            } else
            if (!strcmp(type_slice->arr[i].val, "int")) {
                type.type = CT_INT;
            } else
            if (!strcmp(type_slice->arr[i].val, "long")) {
                type.type = CT_LONG;
            }
        }
        i++;
    }

    return type;
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
        vector_error_t_push_back(parser->err_stk, gen_error("expected `=`, `;` or `(`",
                                                    parser->args.infile_name,
                                                    parser->toks->arr[parser->pos + 1].line_ref,
                                                    parser->toks->arr[parser->pos + 1].chpos_ref));
    }
    while (parser->toks->arr[++parser->pos].type != TT_SEMICOLON) {}
    parser->cur_node = ast_acc;
}

void parser_fun_decl_parse(parser_info_t *parser, size_t ident_offset, size_t se_offset, ctype_t type, token_t ident)
{
    ast_node_t *ast_acc = parser->cur_node;

    ast_fun_info_t *fun_info = (ast_fun_info_t*)malloc(sizeof(ast_fun_info_t));
    fun_info->type = type;
    strcpy(fun_info->name, ident.val);
    fun_info->params = list_ast_var_info_t_create();

    size_t rparent_offset = se_offset + 1;
    while (parser->toks->arr[parser->pos + rparent_offset].type != TT_RPARENT) {
        if (parser->toks->arr[parser->pos + rparent_offset].type != TT_TYPE_NAME) {
            vector_error_t_push_back(parser->err_stk, gen_error("Expected argument type",
                                                        parser->args.infile_name,
                                                        parser->toks->arr[parser->pos + rparent_offset].line_ref,
                                                        parser->toks->arr[parser->pos + rparent_offset].chpos_ref));
            return;
        }
        vector_token_t_t *arg_slice = vector_token_t_create();
        size_t arg_ident_offset = rparent_offset;
        while (parser->toks->arr[parser->pos + rparent_offset].type != TT_COMA
            && parser->toks->arr[parser->pos + rparent_offset].type != TT_RPARENT
        ) {
            if (parser->toks->arr[parser->pos + rparent_offset].type != TT_IDENT) {
                arg_ident_offset = rparent_offset - arg_ident_offset + 1;
            }
            vector_token_t_push_back(arg_slice, parser->toks->arr[parser->pos + rparent_offset]);
            rparent_offset++;
        }
        ast_var_info_t *arg_info = malloc(sizeof(ast_var_info_t));
        arg_info->type = parser_type_parse(parser, arg_slice, arg_ident_offset);
        strcpy(arg_info->name, arg_slice->arr[arg_ident_offset].val);
        list_ast_var_info_t_add(fun_info->params, arg_info);

        if (parser->toks->arr[parser->pos + rparent_offset].type == TT_RPARENT) {
            break;
        }
        rparent_offset++;
    }
    parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_FUNCTION_DECL, fun_info));
    hashmap_ast_fun_info_t_set(fun_infos, fun_info->name, fun_info);

    if (parser->toks->arr[parser->pos + rparent_offset + 1].type == TT_LBRACKET) {
        size_t skip_rb = 0;
        size_t i = rparent_offset + 2;
        for (;;i++) {
            if (parser->toks->arr[parser->pos + i].type == TT_LBRACKET) {
                skip_rb++;
            }
            if (parser->toks->arr[parser->pos + i].type == TT_RBRACKET) {
                if (skip_rb) {
                    skip_rb--;
                    continue;
                }
                break;
            }
        }
        parser_namespace_parse(parser, parser->pos + rparent_offset + 2, parser->pos + i);
    }

    parser->cur_node = ast_acc;
}

void parser_decl_parse(parser_info_t *parser)
{
    vector_token_t_t *type_slice = vector_token_t_create();
    size_t ident_offset = 0;
    while (parser->toks->arr[parser->pos + ident_offset].type != TT_IDENT) {
        vector_token_t_push_back(type_slice, parser->toks->arr[parser->pos + ident_offset]);
        if (parser->pos + ++ident_offset == parser->toks->size) {
            vector_error_t_push_back(parser->err_stk, gen_error("expected identifire",
                                                        parser->args.infile_name,
                                                        parser->toks->arr[parser->pos + 1].line_ref,
                                                        parser->toks->arr[parser->pos + 1].chpos_ref));
            return;
        }
    }
    token_t ident = parser->toks->arr[parser->pos + ident_offset];
    vector_token_t_push_back(type_slice, parser->toks->arr[parser->pos + ident_offset]);

    size_t se_offset = ident_offset + 1;
    while (parser->toks->arr[parser->pos + se_offset].type != TT_EQ
        && parser->toks->arr[parser->pos + se_offset].type != TT_LPARENT
    ) {
        vector_token_t_push_back(type_slice, parser->toks->arr[parser->pos + se_offset]);
        se_offset++;
    }

    ctype_t type = parser_type_parse(parser, type_slice, ident_offset);
    vector_token_t_free(type_slice);

    // printf_s("%s, %u\n", type_slice->arr[ident_offset].val, type.type);

    if (parser->toks->arr[parser->pos + se_offset].type == TT_LPARENT) {
        parser_fun_decl_parse(parser, ident_offset, se_offset, type, ident);
    } else {
        parser_var_decl_parse(parser, ident_offset, se_offset, type, ident);
    }
}

void parser_namespace_parse(parser_info_t *parser, size_t start, size_t end)
{
    for (parser->pos = start; parser->pos < end; parser->pos++) {
        if (parser->toks->arr[parser->pos].type == TT_TYPE_NAME) {
            parser_decl_parse(parser);
        } else
        if (parser->toks->arr[parser->pos].type == TT_KW_RETURN) {
            vector_token_t_t *stmt = vector_token_t_create();
            while (parser->toks->arr[parser->pos].type != TT_SEMICOLON) {
                vector_token_t_push_back(stmt, parser->toks->arr[parser->pos]);
                parser->pos++;
            }
            parser_stmt_return_parse(parser, stmt);
            vector_token_t_free(stmt);
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

#endif