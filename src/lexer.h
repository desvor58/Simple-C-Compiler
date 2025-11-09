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
            lexer_decl_analis(lexer);
        }
    }
}

void lexer_decl_analis(lexer_info_t *lexer)
{
    while (lexer->pos < lexer->toks->size
        && (lexer->toks->arr[lexer->pos].type == TT_TYPE_NAME
         || lexer->toks->arr[lexer->pos].type == TT_STAR)) {
        lexer->pos++;
    }

    if (lexer->pos >= lexer->toks->size) {
        generr(lexer->err_stk,
               "Expected identifire",
               lexer->args.infile_name,
               lexer->toks->arr[lexer->pos - 1].line_ref,
               lexer->toks->arr[lexer->pos - 1].chpos_ref);
    } else
    if (lexer->toks->arr[lexer->pos].type != TT_IDENT) {
        generr(lexer->err_stk,
               "Expected identifire",
               lexer->args.infile_name,
               lexer->toks->arr[lexer->pos].line_ref,
               lexer->toks->arr[lexer->pos].chpos_ref);
    }

    for (;;lexer->pos++) {
        if (lexer->pos >= lexer->toks->size) {
            generr(lexer->err_stk,
                   "Expected `=`, `;` or `(` token",
                   lexer->toks->arr[lexer->pos - 1].file_ref,
                   lexer->toks->arr[lexer->pos - 1].line_ref,
                   lexer->toks->arr[lexer->pos - 1].chpos_ref);
            break;
        }
        if (lexer->toks->arr[lexer->pos].type == TT_SEMICOLON) {
            return;
        }
        if (lexer->toks->arr[lexer->pos].type == TT_EQ
         || lexer->toks->arr[lexer->pos].type == TT_LPARENT) {
            break;
        }
        if (lexer->toks->arr[lexer->pos].type == TT_LSQUARE_BRACKET) {
            if (lexer->toks->arr[++lexer->pos].type != TT_RSQUARE_BRACKET) {
                generr(lexer->err_stk,
                       "Expected `]` token after `[`",
                       lexer->toks->arr[lexer->pos - 1].file_ref,
                       lexer->toks->arr[lexer->pos - 1].line_ref,
                       lexer->toks->arr[lexer->pos - 1].chpos_ref);
            }
        }
    }
    if (lexer->toks->arr[++lexer->pos].type == TT_SEMICOLON) {
        generr(lexer->err_stk,
               "Expected expression before `;` token",
               lexer->toks->arr[lexer->pos - 1].file_ref,
               lexer->toks->arr[lexer->pos - 1].line_ref,
               lexer->toks->arr[lexer->pos - 1].chpos_ref);
    }
}

#endif