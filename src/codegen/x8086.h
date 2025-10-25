#ifndef CODEGEN_X8086_H
#define CODEGEN_X8086_H

#include "x8086/common.h"
#include "x8086/expr.h"
#include "x8086/decl.h"

void codegen_x8086(codegen_x8086_info_t *codegen)
{
    var_offsets = hashmap_codegen_var_info_t_create();

    putoutcode("bits 16\n\nglobal %s\n\n", codegen->args.entry_fun_name);

    putoutcode("section .data\n", 0);
    codegen->outcode_offset++;
    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_VARIABLE_DECL) {
            codegen_x8086_static_var_decl(codegen);
        }
    }
    codegen->outcode_offset--;

    putoutcode("\nsection .text\n", 0);
    codegen->outcode_offset++;

    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_FUNCTION_DECL) {
            codegen_x8086_fun_decl(codegen);
        }
    }
    codegen->outcode_offset--;
}


#endif