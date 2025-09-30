#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdio.h>

#include "config.h"
#include "types/list.h"
#include "types/vector.h"

typedef          char   i8;
typedef          short  i16;
typedef          int    i32;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef          float  f32;
typedef          double f64;


#define ARGS_FLG_ASM_STOP     1
#define ARGS_FLG_PREPROC_STOP 2
#define ARGS_FLG_TOKS_PUT     4
#define ARGS_FLG_AST_PUT      8
#define ARGS_FLG_OBJ_STOP     16

typedef struct {
    char infile_name[64];
    char outfile_name[64];
    int  flags;
} args_t;

typedef struct {
    const char *msg;
    char        file[MAX_IDENT_SIZE];
    size_t      line;
    size_t      chpos;
} error_t;

genvector(error_t, 8);

error_t gen_error(const char *msg, char *file, size_t line, size_t chpos)
{
    error_t err;
    err.msg = msg;
    strcpy(err.file, file);
    err.line = line;
    err.chpos = chpos;
    return err;
}

#define errOK gen_error("", "", 0, 0)

void put_error(error_t err, int fatal)
{
    printf_s("%s:%u:%u:error: %s\n", err.file, err.line, err.chpos, err.msg);
    if (fatal) {
        exit(1);
    }
}


typedef enum {
    CT_CHAR,
    CT_SHORT,
    CT_INT,
    CT_LONG,
    CT_LONG_LONG,
    CT_STRUCT,
} ctype_type;

#define CTYPE_IS_POINTER  1
#define CTYPE_IS_ARRAY    2
#define CTYPE_IS_UNSIGNED 4

typedef enum {
    CTM_POINTER,
    CTM_ARRAY,
    CTM_UNSIGNED,
    CTM_STATIC,
    CTM_CONST,
} ctype_modifire_t;

typedef struct {
    ctype_type type;
    ctype_modifire_t modifires[8];
    size_t           modifires_top;
} ctype_t;


typedef enum {
    TT_TYPE_NAME,
    TT_IDENT,
    TT_EQ,
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_LPARENT,
    TT_RPARENT,
    TT_LBRACKET,
    TT_RBRACKET,
    TT_LSQUARE_BRACKET,
    TT_RSQUARE_BRACKET,
    TT_INT_LIT,
    TT_FLOAT_LIT,
    TT_STR_LIT,
    TT_SEMICOLON,
} token_type;

typedef struct {
    token_type type;
    char       val[MAX_IDENT_SIZE];
    size_t     line_ref;
    size_t     chpos_ref;
} token_t;

genvector(token_t, 128)

token_t gen_token(token_type type, char *val, size_t line_ref, size_t chpos_ref)
{
    token_t tok;
    tok.type = type;
    strcpy(tok.val, val);
    tok.line_ref = line_ref;
    tok.chpos_ref = chpos_ref;
    return tok;
}


typedef enum {
    NT_TRANSLATION_UNIT,
    NT_FUNCTION_DECL,
    NT_VARIABLE_DECL,
    NT_EXPR,
    NT_BOP,
    NT_UOP,
    NT_INT_LIT,
    NT_STR_LIT,
    NT_FLOAT_LIT,
    NT_FUNCTION_CALL,
} ast_node_type;

typedef struct {
    ctype_t type;
    char    name[MAX_IDENT_SIZE];
} ast_var_info_t;

extern struct list_ast_node_t_pair_t;

typedef struct {
    ast_node_type                  type;
    struct list_ast_node_t_pair_t *childs;
    void                          *info;
} ast_node_t;

genlist(ast_node_t);

ast_node_t *gen_ast_node(ast_node_type type, void *info)
{
    ast_node_t *node = malloc(sizeof(ast_node_t));
    node->type   = type;
    node->info   = info;
    node->childs = list_ast_node_t_create();
    return node;
}

ast_node_t *ast_node_add_child(ast_node_t *parent, ast_node_t *child)
{
    list_ast_node_t_add(parent->childs, child);
    return child;
}

void ast_node_delete(ast_node_t *ast)
{
    foreach (list_ast_node_t_pair_t, ast->childs) {
        ast_node_delete(cur->val);
    }

    list_ast_node_t_free(ast->childs);
    if (ast->info) {
        free(ast->info);
    }
    free(ast);
}

// создает навую строку в памяти
// надобно освобождть
char *stralc(char *str)
{
    char *new_str = malloc(sizeof(char) * strlen(str) + 1);
    char *c = str;
    char *n = new_str;
    while (*c) {
        *n = *c;
        n++;
        c++;
    }
    *n = '\0';
    return new_str;
}

int buf_insert(char *buf, size_t buf_size, size_t index, char *insert)
{
    size_t buflen = strlen(buf);
    size_t insertlen = strlen(insert);
    
    if (buf_size < buflen + insertlen
     || index >= buflen) {
        return 1;
    }

    char *save_buf = (char*)malloc(buflen - index);

    size_t i;
    for (i = index; i < buflen; i++) {
        save_buf[i - index] = buf[i];
    }
    save_buf[i] = '\0';
    for (i = 0; i < insertlen; i++) {
        buf[i + index] = insert[i];
    }
    for (i = 0; i < strlen(save_buf); i++) {
        buf[i + index + insertlen] = save_buf[i];
    }
    buf[i] = '\0';

    free(save_buf);

    return 0;
}

int buf_replace(char *buf, size_t buf_size, u32 repl_start, u32 repl_end, char *repl)
{
    size_t buflen = strlen(buf);
    size_t repllen = strlen(repl);

    if (repl_start > repl_end
     || repl_end > buflen
     || buf_size < (buflen - (repl_end - repl_start)) + repllen) {
        return 1;
    }

    char *save_buf = (char*)malloc((buflen - (repl_end - repl_start)) + repllen);
    size_t i = repl_end;
    while (i < buflen) {
        save_buf[i - repl_end] = buf[i];
        i++;
    }
    save_buf[i - repl_end] = '\0';
    for (i = 0; i < repllen; i++) {
        buf[repl_start + i] = repl[i];
    }
    for (i = 0; i < strlen(save_buf); i++) {
        buf[i + repl_start + repllen] = save_buf[i];
    }
    buf[i + repl_start + repllen] = '\0';

    free(save_buf);
    return 0;
}

void put_pos_str(char *str, size_t pos)
{
    for (size_t i = 0; i < strlen(str); i++) {
        if (i == pos) {
            printf_s("$");
        }
        putchar(str[i]);
    }
    putchar('\n');
}

void get_folder_path(char *path, char *buf)
{
    long long top = strlen(buf) + 1;
    while (path[top] != '/' && top >= 0) {
        top--;
    }
    buf[0] = '.';
    buf[1] = '/';
    size_t i = 2;
    while (i - 2 <= top) {
        buf[i++] = path[i - 2];
    }
    buf[i] = '\0';
}

void get_file_text(FILE *file, char *buf)
{
    char c = ' ';
    int i = 0;
    for (; c != EOF; i++) {
        c = getc(file);
        buf[i] = c;
        if (c == '/') {
            c = getc(file);
            if (c == '/') {
                while (c != '\n' && c != EOF) {
                    c = getc(file);
                }
                buf[i] = '\n';
            }
        }
    }
    buf[i] = '\0';
}


typedef struct {
    char name[MAX_IDENT_SIZE];
    char args[MAX_MACRO_ARGS][MAX_IDENT_SIZE];
    char val[MAX_MACRO_VAL];
} macro_info_t;

#endif