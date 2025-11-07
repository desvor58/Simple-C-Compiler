#ifndef CODEGNE_X8664_WIN_DECL_H
#define CODEGNE_X8664_WIN_DECL_H

#include "common.h"
#include "expr.h"

void codegen_x8664_win_namespace_gen(codegen_x8664_win_info_t *codegen, size_t locvar_start)
{
    ast_node_t *body = codegen->cur_node;
    codegen_namespace_info_t *namespace = malloc(sizeof(codegen_namespace_info_t));
    namespace->locvar_offset = locvar_start;
    list_codegen_namespace_info_t_pair_t *new_namespace_list = list_codegen_namespace_info_t_create();
    new_namespace_list->val = namespace;
    new_namespace_list->next = codegen->namespaces;
    codegen->namespaces = new_namespace_list;
    foreach (list_ast_node_t_pair_t, body->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_VARIABLE_DECL) {
            codegen_x8664_win_var_decl(codegen);
        } else {
            codegen_x8664_win_expr_gen(codegen, cur->val, 0);
        }
    }
    codegen->namespaces = codegen->namespaces->next;
    free(new_namespace_list);
    free(namespace);
}

void codegen_x8664_win_var_decl(codegen_x8664_win_info_t *codegen)
{
    ast_var_info_t *var_info = codegen->cur_node->info;
    codegen_var_info_t *cginfo = malloc(sizeof(codegen_var_info_t));
    cginfo->rbp_offset = codegen->namespaces->val->locvar_offset + codegen_x8664_win_get_type_size(var_info->type.type);
    cginfo->isstatic = 0;
    cginfo->type = var_info->type;
    hashmap_codegen_var_info_t_set(var_offsets, var_info->name, cginfo);
    if (codegen->cur_node->childs->val->type == NT_EXPR) {
        codegen_x8664_win_expr_gen(codegen, codegen->cur_node->childs->val->childs->val, 0);
        putoutcode("mov %s [rbp - %u], rax\n",
                                     codegen_x8664_win_get_asm_type(var_info->type.type),
                                     codegen->namespaces->val->locvar_offset += codegen_x8664_win_get_type_size(var_info->type.type));
        return;
    }
    string_t *dst_str = string_create("");
    string_cat(dst_str, "%s [rbp - %u]",
                        codegen_x8664_win_get_asm_type(var_info->type.type),
                        codegen->namespaces->val->locvar_offset += codegen_x8664_win_get_type_size(var_info->type.type));
    string_push_back(dst_str, '\0');
    codegen_x8664_win_get_val(codegen, codegen->cur_node->childs->val, dst_str->str);
    string_free(dst_str);
}

void codegen_x8664_win_fun_decl(codegen_x8664_win_info_t *codegen)
{
    ast_fun_info_t *fun_info = (ast_fun_info_t*)codegen->cur_node->info;
    if (!fun_info) {
        generr(codegen->err_stk,
               "fun declaration node havent ast_fun_info",
               codegen->args.infile_name,
               0,
               0);
        return;
    }
    putoutcode("%s:\n", fun_info->name);
    putoutcode("push rbp\n", 0);
    putoutcode("mov rbp, rsp\n", 0);
    codegen->outcode_offset++;

    size_t offset = 0;
    size_t arg_num = 0;
    foreach (list_ast_var_info_t_pair_t, fun_info->params) {
        putoutcode("mov %s [rbp - %u], %s\n",
                   codegen_x8664_win_get_asm_type(cur->val->type.type),
                   offset += codegen_x8664_win_get_type_size(cur->val->type.type),
                   fun_args_regs[arg_num]);
        codegen_var_info_t *var_info = malloc(sizeof(codegen_var_info_t));
        var_info->isstatic   = 0;
        var_info->type       = fun_info->type;
        var_info->rbp_offset = offset;
        hashmap_codegen_var_info_t_set(var_offsets, cur->val->name, var_info);
        // for (size_t o = 0; o < var_offsets->keys_top; o++) {
        //     printf_s("    %s\n", var_offsets->keys[o]);
        // }

        arg_num++;
    }

    codegen_x8664_win_namespace_gen(codegen, offset);

    codegen->outcode_offset--;
    putoutcode("pop rbp\n", 0);
    putoutcode("ret\n\n", 0);
}

void codegen_x8664_win_static_var_decl(codegen_x8664_win_info_t *codegen)
{
    ast_var_info_t *var_info = (ast_var_info_t*)codegen->cur_node->info;
    codegen_var_info_t *cginfo = malloc(sizeof(codegen_var_info_t));
    cginfo->rbp_offset = 0;
    cginfo->isstatic = 1;
    cginfo->type = var_info->type;
    hashmap_codegen_var_info_t_set(var_offsets, var_info->name, cginfo);
    if (!var_info) {
        generr(codegen->err_stk,
               "var declaration node havent ast_var_info",
               codegen->args.infile_name,
               0,
               0);
        return;
    }
    putoutcode("%s %s %s\n",
               var_info->name,
               codegen_x8664_win_get_static_asm_type(var_info->type.type),
               (char*)codegen->cur_node->childs->val->info);
}
#endif