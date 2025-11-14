#ifndef CODEGEN_X8086_H
#define CODEGEN_X8086_H

#include "x8086/common.h"
#include "x8086/expr.h"
#include "x8086/decl.h"

void codegen_x8086(codegen_x8086_info_t *codegen)
{
    var_offsets = hashmap_codegen_var_info_t_create();

    putoutcode("[BITS 16]\n[ORG 8000h]\n\n", codegen->args.entry_fun_name);

    puts("text start");

    size_t i = 0;
    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_FUNCTION_DECL
         && !strcmp(((ast_fun_info_t*)cur->val->info)->name, codegen->args.entry_fun_name)
        ) {
            codegen_x8086_fun_decl(codegen);
        }
        i++;
    }

    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_FUNCTION_DECL
         && strcmp(((ast_fun_info_t*)cur->val->info)->name, codegen->args.entry_fun_name)
        ) {
            codegen_x8086_fun_decl(codegen);
        }
    }

    puts("data start");

    size_t lit_num = 0;
    foreach (list_string_t_pair_t, codegen->data_str_lits) {
        printf_s("%u  str:%s\n", lit_num++, cur->val->str);
        string_cat(codegen->outcode, "strlit%u db \"%s\", 0\n", lit_num++, cur->val->str);
        printf_s("end str\n");
    }
    puts("strlits end");
    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_VARIABLE_DECL) {
            codegen_x8086_static_var_decl(codegen);
        }
    }
    puts("data var end");
    string_push_back(codegen->outcode, '\0');
}


#endif