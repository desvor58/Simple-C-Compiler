#ifndef PARSER_H
#define PARSER_H

#include "parser/common.h"
#include "parser/expr.h"
#include "parser/decl.h"

void parser_expr_parse(parser_info_t *parser, vector_token_t_t *expr);
void parser_decl_parse(parser_info_t *parser);

void parser_namespace_parse(parser_info_t *parser, size_t start, size_t end);

void parse(parser_info_t *parser)
{
    parser_namespace_parse(parser, 0, parser->toks->size);
}

#endif