#ifndef PARSER_H
#define PARSER_H

#include "types.h"

typedef struct {
    args_t      args;
    token_t    *toks;
    size_t      toks_top;
    ast_node_t *ast_root;
    ast_node_t *cur_node;
    token_t    *buf;
    size_t      pos;
} parser_info_t;

void parser_create(parser_info_t *parser, args_t args, token_t *toks, size_t toks_top, char *file)
{
    parser->args     = args;
    parser->toks     = toks;
    parser->toks_top = toks_top;
    parser->ast_root = gen_ast_node(NT_TRANSLATION_UNIT, (void*)0);
    parser->cur_node = parser->ast_root;
    parser->buf      = (token_t*)malloc(sizeof(token_t) * 16);
    parser->pos      = 0;
}

void parser_delete(parser_info_t *parser)
{
    ast_node_delete(parser->ast_root);
    free(parser->buf);
}

void parser_decl_parse(parser_info_t *parser);

static error_stk_t *err_stk;

void parse(parser_info_t *parser)
{
    for (parser->pos = 0; parser->pos < parser->toks_top; parser->pos++) {
        if (parser->toks[parser->pos].type = TT_TYPE_NAME) {
            parser_decl_parse(parser);
        }
    }
}

void parser_get_val(parser_info_t *parser, size_t offset)
{
    if (parser->toks[parser->pos + offset].type == TT_INT_LIT) {
        ast_node_add_child(parser->cur_node, gen_ast_node(NT_INT_LIT, parser->toks[parser->pos + offset].val));
    } else
    if (parser->toks[parser->pos + offset].type == TT_STR_LIT) {
        ast_node_add_child(parser->cur_node, gen_ast_node(NT_STR_LIT, parser->toks[parser->pos + offset].val));
    } else {
        exit(76);
    }
}

void parser_expr_parse(parser_info_t *parser, token_t *buf)
{
    if (parser->toks[parser->pos + offset + 1].type != TT_LPARENT
     || parser->toks[parser->pos + offset + 1].type != TT_LBRACKET
     || parser->toks[parser->pos + offset + 1].type != TT_LSQUARE_BRACKET
     || parser->toks[parser->pos + offset + 1].type != TT_SEMICOLON
    ) {
        parser_get_val(parser, offset);
        return;
    }
    ast_node_t *ast_acc = parser->cur_node;
    size_t i = 0;
    while (parser->toks[parser->pos + offset + i].type != TT_LPARENT
        && parser->toks[parser->pos + offset + i].type != TT_LBRACKET
        && parser->toks[parser->pos + offset + i].type != TT_LSQUARE_BRACKET
        && parser->toks[parser->pos + offset + i].type != TT_SEMICOLON
    ) {
        if (parser->toks[parser->pos + offset + i].type == TT_STAR) {
            parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_BOP, "*"));
        } else
        if (parser->toks[parser->pos + offset + 1].type == TT_SLASH) {
            parser->cur_node = ast_node_add_child(parser->cur_node, gen_ast_node(NT_BOP, "/"));
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

    if (parser->toks[parser->pos + offset].type == TT_EQ) {
        
    } else
    if (parser->toks[parser->pos = offset].type == TT_SEMICOLON) {
        return;
    } else {
        err_stk->stk[err_stk->top++] = gen_error("expected `=`, `;` or `(`",
                                                 parser->args.infile_name,
                                                 parser->toks[parser->pos + 1].line_ref,
                                                 parser->toks[parser->pos + 1].chpos_ref);
    }
}

void parser_decl_parse(parser_info_t *parser)
{
    size_t offset = 1;
    while (parser->toks[parser->pos + offset].type != TT_IDENT) {
        if (parser->pos + ++offset == parser->toks_top) {
            err_stk->stk[err_stk->top++] = gen_error("expected identifire",
                                                     parser->args.infile_name,
                                                     parser->toks[parser->pos + 1].line_ref,
                                                     parser->toks[parser->pos + 1].chpos_ref);
            return;
        }
    }
    token_t ident = parser->toks[parser->pos + offset];
    ctype_t type = {0};
    type.modifires_top = 0;
    if (!strcmp(parser->toks[parser->pos].val, "char")) {
        type.type = CT_CHAR;
    } else
    if (!strcmp(parser->toks[parser->pos].val, "short")) {
        type.type = CT_SHORT;
    } else
    if (!strcmp(parser->toks[parser->pos].val, "int")) {
        type.type = CT_INT;
    } else
    if (!strcmp(parser->toks[parser->pos].val, "long")) {
        type.type = CT_LONG;
    }
    size_t i = 1;
    while (parser->toks[parser->pos + offset + i].type != TT_EQ
        || parser->toks[parser->pos + offset + i].type != TT_LPARENT
    ) {
        if (parser->toks[parser->pos + offset + i].type == TT_LSQUARE_BRACKET) {
            if (parser->toks[parser->pos + offset + i + 1].type == TT_RSQUARE_BRACKET) {
                type.modifires[type.modifires_top++] = CTM_ARRAY;
            } else {
                err_stk->stk[err_stk->top++] = gen_error("expected right bracket",
                                                         parser->args.infile_name,
                                                         parser->toks[parser->pos + 1].line_ref,
                                                         parser->toks[parser->pos + 1].chpos_ref);
            }
        } else {
            err_stk->stk[err_stk->top++] = gen_error("unknow type modifire",
                                                     parser->args.infile_name,
                                                     parser->toks[parser->pos + 1].line_ref,
                                                     parser->toks[parser->pos + 1].chpos_ref);
        }
        i++;
    }
    i = -1;
    while (parser->toks[parser->pos + offset + i].type != TT_EQ
        || parser->toks[parser->pos + offset + i].type != TT_LPARENT
    ) {
        if (parser->toks[parser->pos + offset + i].type == TT_STAR) {
            type.modifires[type.modifires_top++] = CTM_POINTER;
        } else
        if (parser->toks[parser->pos + offset + i].type == TT_TYPE_NAME) {
            if (!strcmp(parser->toks[parser->pos + offset + i].val, "const")) {
                type.modifires[type.modifires_top++] = CTM_CONST;
            } else
            if (!strcmp(parser->toks[parser->pos + offset + i].val, "static")) {
                type.modifires[type.modifires_top++] = CTM_STATIC;
            } else
            if (!strcmp(parser->toks[parser->pos + offset + i].val, "unsigned")) {
                type.modifires[type.modifires_top++] = CTM_UNSIGNED;
            } else {
                err_stk->stk[err_stk->top++] = gen_error("unknow type modifire",
                                                         parser->args.infile_name,
                                                         parser->toks[parser->pos + 1].line_ref,
                                                         parser->toks[parser->pos + 1].chpos_ref);
            }
        } else {
            err_stk->stk[err_stk->top++] = gen_error("unknow type modifire",
                                                     parser->args.infile_name,
                                                     parser->toks[parser->pos + 1].line_ref,
                                                     parser->toks[parser->pos + 1].chpos_ref);
        }
        i--;
    }

    if (parser->toks[parser->pos + 2].type != TT_LPARENT) {
        // TODO
    }
    parser_var_decl_parse(parser, offset, type, ident);

}

#endif