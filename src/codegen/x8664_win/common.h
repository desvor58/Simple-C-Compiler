#ifndef CODEGEN_X8664_WIN_TYPES_H
#define CODEGEN_X8664_WIN_TYPES_H

#include "../../types.h"
#include "../../types/hashmap.h"

typedef struct {
    size_t locvar_offset;
} codegen_namespace_info_t;
genlist(codegen_namespace_info_t);

typedef struct {
    size_t  rbp_offset;
    ctype_t type;
    int     isstatic;
} codegen_var_info_t;
genhashmap(codegen_var_info_t)

typedef struct {
    vector_error_t_t                     *err_stk;
    args_t                                args;
    ast_node_t                           *ast_root;
    ast_node_t                           *cur_node;
    string_t                             *outcode;
    size_t                                outcode_offset;
    list_codegen_namespace_info_t_pair_t *namespaces;
} codegen_x8664_win_info_t;

void codegen_x8664_win_var_decl(codegen_x8664_win_info_t *codegen);
void codegen_x8664_win_static_var_decl(codegen_x8664_win_info_t *codegen);
void codegen_x8664_win_fun_decl(codegen_x8664_win_info_t *codegen);
void codegen_x8664_win_namespace_gen(codegen_x8664_win_info_t *codegen, size_t locvar_start);

codegen_x8664_win_info_t *codegen_x8664_win_create(args_t args, ast_node_t *ast)
{
    codegen_x8664_win_info_t *codegen = (codegen_x8664_win_info_t*)malloc(sizeof(codegen_x8664_win_info_t));
    codegen->args = args;
    codegen->ast_root = ast;
    codegen->cur_node = ast;
    codegen->outcode = string_create();
    codegen->outcode_offset = 0;
    codegen->namespaces = list_codegen_namespace_info_t_create();
    codegen_namespace_info_t *global_namespace = malloc(sizeof(codegen_namespace_info_t));
    global_namespace->locvar_offset = 0;
    list_codegen_namespace_info_t_add(codegen->namespaces, global_namespace);
    return codegen;
}

void codegen_x8664_win_delete(codegen_x8664_win_info_t *codegen)
{
    foreach (list_codegen_namespace_info_t_pair_t, codegen->namespaces) {
        free(cur->val);
    }
    list_codegen_namespace_info_t_free(codegen->namespaces);
    string_free(codegen->outcode);
}

hashmap_codegen_var_info_t_t *var_offsets;

#define putoutcode(fmt, ...)  \
    for (size_t outcode_offset_counter = 0; outcode_offset_counter < codegen->outcode_offset; outcode_offset_counter++)  \
        string_cat(codegen->outcode, "    ");  \
    string_cat(codegen->outcode, fmt, __VA_ARGS__)

size_t codegen_x8664_win_get_type_size(ctype_type ctype)
{
    if (ctype == CT_CHAR) {
        return 1;
    } else
    if (ctype == CT_SHORT) {
        return 2;
    } else
    if (ctype == CT_INT) {
        return 4;
    } else
    if (ctype == CT_LONG) {
        return 8;
    }
    return 0;
}

char *codegen_x8664_win_get_static_asm_type(ctype_type ctype)
{
    if (ctype == CT_CHAR) {
        return "db";
    } else
    if (ctype == CT_SHORT) {
        return "dw";
    } else
    if (ctype == CT_INT) {
        return "dd";
    } else
    if (ctype == CT_LONG) {
        return "dq";
    }
}

char *codegen_x8664_win_get_asm_type(ctype_type ctype)
{
    if (ctype == CT_CHAR) {
        return "byte";
    } else
    if (ctype == CT_SHORT) {
        return "word";
    } else
    if (ctype == CT_INT) {
        return "dword";
    } else
    if (ctype == CT_LONG) {
        return "qword";
    }
}

#endif