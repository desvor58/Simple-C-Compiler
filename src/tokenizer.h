#ifndef TOKENIZER_H
#define TOKENIZER_H

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
} tokenizer_info_t;

tokenizer_info_t *tokenizer_create(args_t args, char *text, vector_token_t_t *toks, char *file)
{
    tokenizer_info_t *tokenizer = malloc(sizeof(tokenizer_info_t));
    tokenizer->args         = args;
    tokenizer->text         = text;
    tokenizer->toks         = toks;
    tokenizer->buf          = (char*)malloc(sizeof(char) * MAX_IDENT_SIZE);
    tokenizer->pos          = 0;
    tokenizer->file         = file;
    tokenizer->line         = 1;
    tokenizer->chpos        = 0;
    return tokenizer;
}

void tokenizer_delete(tokenizer_info_t *tokenizer)
{
    free(tokenizer->buf);
    free(tokenizer);
}

void tokenizer_alpha_parse(tokenizer_info_t *tokenizer);
void tokenizer_digit_parse(tokenizer_info_t *tokenizer, size_t offset);
void tokenizer_buf_analis(tokenizer_info_t *tokenizer);

#define push_tok(type, val) vector_token_t_push_back(tokenizer->toks, gen_token(type, val, tokenizer->line, tokenizer->chpos))

void tokenize_text(tokenizer_info_t *tokenizer)
{
    for (tokenizer->pos = 0; tokenizer->pos < strlen(tokenizer->text); tokenizer->pos++) {
        if (tokenizer->text[tokenizer->pos] == '\n') {
            tokenizer->line++;
            tokenizer->chpos = 1;
        } else {
            tokenizer->chpos++;
        }
        if (tokenizer->text[tokenizer->pos] == '(') {
            push_tok(TT_LPARENT, "(");
        } else
        if (tokenizer->text[tokenizer->pos] == ')') {
            push_tok(TT_RPARENT, ")");
        } else
        if (tokenizer->text[tokenizer->pos] == '{') {
            push_tok(TT_LBRACKET, "{");
        } else
        if (tokenizer->text[tokenizer->pos] == '}') {
            push_tok(TT_RBRACKET, "}");
        } else
        if (tokenizer->text[tokenizer->pos] == '[') {
            push_tok(TT_LSQUARE_BRACKET, "[");
        } else
        if (tokenizer->text[tokenizer->pos] == ']') {
            push_tok(TT_RSQUARE_BRACKET, "]");
        } else
        if (tokenizer->text[tokenizer->pos] == ';') {
            push_tok(TT_SEMICOLON, ";");
        } else
        if (tokenizer->text[tokenizer->pos] == '=') {
            push_tok(TT_EQ, "=");
        } else
        if (tokenizer->text[tokenizer->pos] == '+') {
            push_tok(TT_PLUS, "+");
        } else
        if (tokenizer->text[tokenizer->pos] == '-') {
            if (tokenizer->toks->arr[tokenizer->toks->size - 1].type != TT_IDENT
             && tokenizer->toks->arr[tokenizer->toks->size - 1].type != TT_INT_LIT
             && tokenizer->toks->arr[tokenizer->toks->size - 1].type != TT_STR_LIT
             && tokenizer->toks->arr[tokenizer->toks->size - 1].type != TT_FLOAT_LIT
             && tokenizer->toks->arr[tokenizer->toks->size - 1].type != TT_RPARENT
            ) {
                tokenizer->buf[0] = '-';
                tokenizer->pos++;
                while (tokenizer->text[tokenizer->pos] == ' ') {tokenizer->pos++;}
                if (isdigit(tokenizer->text[tokenizer->pos])) {
                    tokenizer_digit_parse(tokenizer, 1);
                    tokenizer->pos--;
                    continue;
                }
                tokenizer->pos--;
            }
            push_tok(TT_MINUS, "-");
        } else
        if (tokenizer->text[tokenizer->pos] == '*') {
            push_tok(TT_STAR, "*");
        } else
        if (tokenizer->text[tokenizer->pos] == '/') {
            push_tok(TT_SLASH, "/");
        } else
        if (tokenizer->text[tokenizer->pos] == ',') {
            push_tok(TT_COMA, ",");
        } else
        if (tokenizer->text[tokenizer->pos] == '&') {
            push_tok(TT_AMPERSAND, "&");
        } else
        if (tokenizer->text[tokenizer->pos] == '"') {
            size_t i = 0;
            while (tokenizer->text[++tokenizer->pos] != '"') {
                tokenizer->buf[i++] = tokenizer->text[tokenizer->pos];
            }
            tokenizer->buf[i] = '\0';
            push_tok(TT_STR_LIT, tokenizer->buf);
        } else
        if (isalpha(tokenizer->text[tokenizer->pos]) || tokenizer->text[tokenizer->pos] == '_') {
            tokenizer_alpha_parse(tokenizer);
            tokenizer->pos--;
        } else
        if (isdigit(tokenizer->text[tokenizer->pos])) {
            tokenizer_digit_parse(tokenizer, 0);
            tokenizer->pos--;
        }
    }
}

void tokenizer_alpha_parse(tokenizer_info_t *tokenizer)
{
    size_t i = 0;
    while (isalpha(tokenizer->text[tokenizer->pos])
        || isdigit(tokenizer->text[tokenizer->pos])
        || tokenizer->text[tokenizer->pos] == '_'
    ) {
        tokenizer->buf[i++] = tokenizer->text[tokenizer->pos++];
    }
    tokenizer->buf[i] = '\0';
    tokenizer_buf_analis(tokenizer);
}

void tokenizer_buf_analis(tokenizer_info_t *tokenizer)
{
    if (!strcmp(tokenizer->buf, "char")
     || !strcmp(tokenizer->buf, "short")
     || !strcmp(tokenizer->buf, "int")
     || !strcmp(tokenizer->buf, "long")
     || !strcmp(tokenizer->buf, "signed")
     || !strcmp(tokenizer->buf, "unsigned")
    ) {
        push_tok(TT_TYPE_NAME, tokenizer->buf);
    } else
    if (!strcmp(tokenizer->buf, "return")) {
        push_tok(TT_KW_RETURN, "return");
    } else {
        push_tok(TT_IDENT, tokenizer->buf);
    }
}

void tokenizer_digit_parse(tokenizer_info_t *tokenizer, size_t offset)
{
    size_t i = offset;
    while (isdigit(tokenizer->text[tokenizer->pos])) {
        tokenizer->buf[i++] = tokenizer->text[tokenizer->pos++];
    }
    tokenizer->buf[i] = '\0';
    push_tok(TT_INT_LIT, tokenizer->buf);
}

#endif