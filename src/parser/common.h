#ifndef PARSER_COMMON_H
#define PARSER_COMMON_H

#include "../types.h"

typedef struct {
    list_error_t_pair_t *err_stk;
    args_t               args;
    vector_token_t_t    *toks;
    ast_node_t          *ast_root;
    ast_node_t          *cur_node;
    size_t               pos;
} parser_info_t;

genhashmap(ast_fun_info_t);

hashmap_ast_fun_info_t_t *fun_infos;

void parser_expr_parse(parser_info_t *parser, vector_token_t_t *expr);
int parser_decl_parse(parser_info_t *parser);
void parser_namespace_parse(parser_info_t *parser, size_t start, size_t end);

parser_info_t *parser_create(list_error_t_pair_t *err_stk, args_t args, vector_token_t_t *toks, char *file)
{
    parser_info_t *parser = malloc(sizeof(parser_info_t));
    parser->err_stk       = err_stk;
    parser->args          = args;
    parser->toks          = toks;
    parser->ast_root      = gen_ast_node(NT_TRANSLATION_UNIT, (void*)0);
    parser->cur_node      = parser->ast_root;
    parser->pos           = 0;

    fun_infos = hashmap_ast_fun_info_t_create();

    return parser;
}

void parser_delete(parser_info_t *parser)
{
    hashmap_ast_fun_info_t_free(fun_infos);
    ast_node_delete(parser->ast_root);
    free(parser);
}

#endif