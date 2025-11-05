#ifndef CODEGEN_X8086_WIN_COMMON_H
#define CODEGEN_X8086_WIN_COMMON_H

#include "../../types.h"
#include "../../types/hashmap.h"
#include "../codegen_common.h"

genlist(string_t)

typedef struct {
    list_error_t_pair_t                  *err_stk;
    args_t                                args;
    ast_node_t                           *ast_root;
    ast_node_t                           *cur_node;
    string_t                             *outcode;
    size_t                                outcode_offset;
    list_codegen_namespace_info_t_pair_t *namespaces;
    ast_fun_info_t                       *cur_fun_info;
    list_string_t_pair_t                 *data_str_lits;
    size_t                                data_str_lit_count;
} codegen_x8086_info_t;

void codegen_x8086_var_decl(codegen_x8086_info_t *codegen);
void codegen_x8086_static_var_decl(codegen_x8086_info_t *codegen);
void codegen_x8086_fun_decl(codegen_x8086_info_t *codegen);
void codegen_x8086_namespace_gen(codegen_x8086_info_t *codegen, size_t locvar_start);

codegen_x8086_info_t *codegen_x8086_create(list_error_t_pair_t *err_stk, args_t args, ast_node_t *ast)
{
    codegen_x8086_info_t *codegen = malloc(sizeof(codegen_x8086_info_t));
    codegen->err_stk = err_stk;
    codegen->args = args;
    codegen->ast_root = ast;
    codegen->cur_node = ast;
    codegen->outcode = string_create();
    codegen->outcode_offset = 0;
    codegen->namespaces = list_codegen_namespace_info_t_create();
    codegen_namespace_info_t *global_namespace = malloc(sizeof(codegen_namespace_info_t));
    global_namespace->locvar_offset = 0;
    list_codegen_namespace_info_t_add(codegen->namespaces, global_namespace);
    codegen->cur_fun_info = 0;
    codegen->data_str_lits = list_string_t_create();
    return codegen;
}

void codegen_x8086_delete(codegen_x8086_info_t *codegen)
{
    foreach (list_codegen_namespace_info_t_pair_t, codegen->namespaces) {
        free(cur->val);
    }
    list_codegen_namespace_info_t_free(codegen->namespaces);
    string_free(codegen->outcode);
    list_string_t_free(codegen->data_str_lits);
}

hashmap_codegen_var_info_t_t *var_offsets;

#define putoutcode(fmt, ...)  \
    for (size_t outcode_offset_counter = 0; outcode_offset_counter < codegen->outcode_offset; outcode_offset_counter++)  \
        string_cat(codegen->outcode, "    ");  \
    string_cat(codegen->outcode, fmt, __VA_ARGS__)

size_t codegen_x8086_get_type_size(ctype_t ctype)
{
    if (ctype.modifires[0] == CTM_POINTER) {
        return 2;
    }
    if (ctype.type == CT_CHAR) {
        return 1;
    } else
    if (ctype.type == CT_SHORT) {
        return 2;
    } else
    if (ctype.type == CT_INT) {
        return 2;
    }
    return 0;
}

char *codegen_x8086_get_static_asm_type(ctype_t ctype)
{
    if (ctype.modifires[0] == CTM_POINTER) {
        return "dw";
    }
    if (ctype.type == CT_CHAR) {
        return "db";
    } else
    if (ctype.type == CT_SHORT) {
        return "dw";
    } else
    if (ctype.type == CT_INT) {
        return "dw";
    }
    return "";
}

char *codegen_x8086_get_asm_type(ctype_t ctype)
{
    if (ctype.modifires[0] == CTM_POINTER) {
        return "word";
    }
    if (ctype.type == CT_CHAR) {
        return "byte";
    } else
    if (ctype.type == CT_SHORT) {
        return "word";
    } else
    if (ctype.type == CT_INT) {
        return "word";
    }
}

void codegen_x8086_get_reg(char *res, char R, ctype_t type)
{
    res[0] = R;
    if (type.type == CT_CHAR) {
        res[1] = 'l';
    }
    if (type.type == CT_SHORT || type.type == CT_INT || type.modifires[0] == CTM_POINTER){
        res[1] = 'x';
    }
    res[2] = '\0';
}

#endif