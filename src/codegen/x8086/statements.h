#ifndef CODEGNE_X8086_STATEMENTS_H
#define CODEGNE_X8086_STATEMENTS_H

#include "common.h"
#include "expr.h"

void codegen_x8086_stmt_return_gen(codegen_x8086_info_t *codegen)
{
    char *reg = alloca(3);
    codegen_x8086_get_reg(reg, 'a', codegen->cur_fun_info->type);
    if (codegen->cur_node->childs->val->type == NT_EXPR) {
        codegen_x8086_expr_gen(codegen,
                               codegen->cur_node->childs->val->childs->val,
                               codegen->namespaces->val->locvar_offset,
                               codegen->cur_fun_info->type);
        putoutcode("mov %s, [bp - %u]\n", reg, codegen->namespaces->val->locvar_offset);
    } else {
        putoutcode("mov %s, %s\n", reg, codegen->cur_node->childs->val->info);
    }
    putoutcode("pop bp\n", 0);
    putoutcode("ret\n", 0);
}

void codegen_x8086_stmt_asm_gen(codegen_x8086_info_t *codegen)
{
    foreach (list_ast_node_t_pair_t, codegen->cur_node->childs) {
        putoutcode("%s\n", cur->val->info);
    }
}

#endif