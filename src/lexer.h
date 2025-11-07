#ifndef LEXER_H
#define LEXER_H

#include "types.h"

typedef struct {
    list_error_t_pair_t *err_stk;
    args_t               args;
    vector_token_t_t    *toks;
    size_t               pos;
} lexer_info_t;

lexer_info_t *lexer_create(list_error_t_pair_t *err_stk, args_t args, vector_token_t_t *toks)
{
    lexer_info_t *lexer = malloc(sizeof(lexer_info_t));
    lexer->err_stk      = err_stk;
    lexer->args         = args;
    lexer->toks         = toks;
    lexer->pos          = 0;
    return lexer;
}

void lexer_delete(lexer_info_t *lexer)
{
    free(lexer);
}

void lexer_decl_analis(lexer_info_t *lexer);

void lex_text(lexer_info_t *lexer)
{
    for (; lexer->pos < lexer->toks->size; lexer->pos++) {
        if (lexer->toks->arr[lexer->pos].type == TT_TYPE_NAME) {

        }
    }
}

void lexer_decl_analis(lexer_info_t *lexer)
{
    size_t offset = 0;
    while (lexer->pos < lexer->toks
        && (lexer->toks->arr[lexer->pos + offset].type == TT_TYPE_NAME
         || lexer->toks->arr[lexer->pos + offset].type == TT_STAR)) {
        offset++;
    }

    if (lexer->pos + offset >= lexer->toks->size
     || lexer->toks->arr[lexer->pos + offset].type != TT_IDENT) {
        generr(lexer->err_stk,
               "Expected identifire",
               lexer->args.infile_name,
               lexer->toks->arr[lexer->pos + offset].line_ref,
               lexer->toks->arr[lexer->pos + offset].chpos_ref);
    }

    offset++;
    while (lexer->pos < lexer->toks) {
        if (lexer->toks->arr[lexer->pos + offset].type == TT_EQ
         || lexer->toks->arr[lexer->pos + offset].type == TT_SEMICOLON
         || lexer->toks->arr[lexer->pos + offset].type == TT_) {
            break;
        }
        if (lexer->toks->arr[lexer->pos + offset].type == TT_LSQUARE_BRACKET) {
            if (lexer->toks->arr[lexer->pos + offset + 1].type == )
        }
    }
    if (lexer->toks->arr[lexer->pos + 2].type 
     || lexer->pos + 1 >= lexer->toks->size) {
        generr(lexer->err_stk,
               "Expected identifire",
               lexer->args.infile_name,
               lexer->toks->arr[lexer->pos + 1].line_ref,
               lexer->toks->arr[lexer->pos + 1].chpos_ref);
    }
}

#endif