#ifndef CODEGNE_X8086_STATEMENTS_H
#define CODEGNE_X8086_STATEMENTS_H

#include "common.h"
#include "expr.h"

void codegen_x8086_stmt_return_gen(codegen_x8086_info_t *codegen)
{
    if (codegen->cur_node->childs->val->type == NT_EXPR) {
        codegen_x8086_expr_gen(codegen,
                               codegen->cur_node->childs->val->childs->val,
                               codegen->namespaces->val->locvar_offset,
                               codegen->cur_fun_info->type);
        putoutcode("mov ax, [bp - 2]\n", 0);
    } else {
        putoutcode("mov ax, %s\n", codegen->cur_node->childs->val->info);
    }
    putoutcode("pop bp\n", 0);
    putoutcode("ret\n", 0);
}

#endif