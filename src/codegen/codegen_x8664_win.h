#ifndef CODEGEN_X8664_WIN_H
#define CODEGEN_X8664_WIN_H

#include "x8664_win/common.h"
#include "x8664_win/expr.h"
#include "x8664_win/decl.h"

void codegen_x8664_win(codegen_x8664_win_info_t *codegen)
{
    var_offsets = hashmap_codegen_var_info_t_create();

    putoutcode("bits 64\n\nglobal %s\n\n", codegen->args.entry_fun_name);

    putoutcode("section .data\n", 0);
    codegen->outcode_offset++;
    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_VARIABLE_DECL) {
            codegen_x8664_win_static_var_decl(codegen);
        }
    }
    codegen->outcode_offset--;

    putoutcode("\nsection .text\n", 0);
    codegen->outcode_offset++;

    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_FUNCTION_DECL) {
            codegen_x8664_win_fun_decl(codegen);
        }
    }
    codegen->outcode_offset--;
}

#endif