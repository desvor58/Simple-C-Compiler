#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

#include "../types.h"

typedef struct {
    vector_error_t_t *err_stk;
    args_t            args;
    vector_token_t_t *toks;
    ast_node_t       *ast_root;
    ast_node_t       *cur_node;
    size_t            pos;
} parser_info_t;

void parser_expr_parse(parser_info_t *parser, vector_token_t_t *expr);
void parser_decl_parse(parser_info_t *parser);
void parser_namespace_parse(parser_info_t *parser, size_t start, size_t end);

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
#endif