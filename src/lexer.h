#ifndef LEXER_H
#define LEXER_H

#include "types.h"

typedef struct {
    args_t   args;
    char    *text;
    vector_token_t_t *toks;
    char    *buf;
    size_t   pos;
    char    *file;
    size_t   line;
    size_t   chpos;
} lexer_info_t;

lexer_info_t *lexer_create(args_t args, char *text, vector_token_t_t *toks, char *file)
{
    lexer_info_t *lexer = malloc(sizeof(lexer_info_t));
    lexer->args         = args;
    lexer->text         = text;
    lexer->toks         = toks;
    lexer->buf          = (char*)malloc(sizeof(char) * MAX_IDENT_SIZE);
    lexer->pos          = 0;
    lexer->file         = file;
    lexer->line         = 1;
    lexer->chpos        = 0;
    return lexer;
}

void lexer_delete(lexer_info_t *lexer)
{
    free(lexer->buf);
    free(lexer);
}

void lexer_alpha_parse(lexer_info_t *lexer);
void lexer_digit_parse(lexer_info_t *lexer, size_t offset);
void lexer_buf_analis(lexer_info_t *lexer);

#define push_tok(type, val) vector_token_t_push_back(lexer->toks, gen_token(type, val, lexer->line, lexer->chpos))

void lex_text(lexer_info_t *lexer)
{
    for (lexer->pos = 0; lexer->pos < strlen(lexer->text); lexer->pos++) {
        if (lexer->text[lexer->pos] == '\n') {
            lexer->line++;
            lexer->chpos = 1;
        } else {
            lexer->chpos++;
        }
        if (lexer->text[lexer->pos] == '(') {
            push_tok(TT_LPARENT, "(");
        } else
        if (lexer->text[lexer->pos] == ')') {
            push_tok(TT_RPARENT, ")");
        } else
        if (lexer->text[lexer->pos] == '{') {
            push_tok(TT_LBRACKET, "{");
        } else
        if (lexer->text[lexer->pos] == '}') {
            push_tok(TT_RBRACKET, "}");
        } else
        if (lexer->text[lexer->pos] == '[') {
            push_tok(TT_LSQUARE_BRACKET, "[");
        } else
        if (lexer->text[lexer->pos] == ']') {
            push_tok(TT_RSQUARE_BRACKET, "]");
        } else
        if (lexer->text[lexer->pos] == ';') {
            push_tok(TT_SEMICOLON, ";");
        } else
        if (lexer->text[lexer->pos] == '=') {
            push_tok(TT_EQ, "=");
        } else
        if (lexer->text[lexer->pos] == '+') {
            push_tok(TT_PLUS, "+");
        } else
        if (lexer->text[lexer->pos] == '-') {
            if (lexer->toks->arr[lexer->toks->size - 1].type != TT_IDENT
             && lexer->toks->arr[lexer->toks->size - 1].type != TT_INT_LIT
             && lexer->toks->arr[lexer->toks->size - 1].type != TT_STR_LIT
             && lexer->toks->arr[lexer->toks->size - 1].type != TT_FLOAT_LIT
             && lexer->toks->arr[lexer->toks->size - 1].type != TT_RPARENT
            ) {
                lexer->buf[0] = '-';
                lexer->pos++;
                while (lexer->text[lexer->pos] == ' ') {lexer->pos++;}
                if (isdigit(lexer->text[lexer->pos])) {
                    lexer_digit_parse(lexer, 1);
                    lexer->pos--;
                    continue;
                }
                lexer->pos--;
            }
            push_tok(TT_MINUS, "-");
        } else
        if (lexer->text[lexer->pos] == '*') {
            push_tok(TT_STAR, "*");
        } else
        if (lexer->text[lexer->pos] == '/') {
            push_tok(TT_SLASH, "/");
        } else
        if (lexer->text[lexer->pos] == ',') {
            push_tok(TT_COMA, ",");
        } else
        if (lexer->text[lexer->pos] == '&') {
            push_tok(TT_AMPERSAND, "&");
        } else
        if (lexer->text[lexer->pos] == '"') {
            size_t i = 0;
            while (lexer->text[++lexer->pos] != '"') {
                lexer->buf[i++] = lexer->text[lexer->pos];
            }
            lexer->buf[i] == '\0';
            push_tok(TT_STR_LIT, lexer->buf);
        } else
        if (isalpha(lexer->text[lexer->pos]) || lexer->text[lexer->pos] == '_') {
            lexer_alpha_parse(lexer);
            lexer->pos--;
        } else
        if (isdigit(lexer->text[lexer->pos])) {
            lexer_digit_parse(lexer, 0);
            lexer->pos--;
        }
    }
}

void lexer_alpha_parse(lexer_info_t *lexer)
{
    size_t i = 0;
    while (isalpha(lexer->text[lexer->pos])
        || isdigit(lexer->text[lexer->pos])
        || lexer->text[lexer->pos] == '_'
    ) {
        lexer->buf[i++] = lexer->text[lexer->pos++];
    }
    lexer->buf[i] = '\0';
    lexer_buf_analis(lexer);
}

void lexer_buf_analis(lexer_info_t *lexer)
{
    if (!strcmp(lexer->buf, "char")
     || !strcmp(lexer->buf, "short")
     || !strcmp(lexer->buf, "int")
     || !strcmp(lexer->buf, "long")
     || !strcmp(lexer->buf, "signed")
     || !strcmp(lexer->buf, "unsigned")
    ) {
        push_tok(TT_TYPE_NAME, lexer->buf);
    } else
    if (!strcmp(lexer->buf, "return")) {
        push_tok(TT_KW_RETURN, "return");
    } else {
        push_tok(TT_IDENT, lexer->buf);
    }
}

void lexer_digit_parse(lexer_info_t *lexer, size_t offset)
{
    size_t i = offset;
    while (isdigit(lexer->text[lexer->pos])) {
        lexer->buf[i++] = lexer->text[lexer->pos++];
    }
    lexer->buf[i] = '\0';
    push_tok(TT_INT_LIT, lexer->buf);
}

#endif