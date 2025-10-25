#ifndef CODEGNE_X8664_WIN_EXPR_H
#define CODEGNE_X8664_WIN_EXPR_H

#include "common.h"

static char *expr_regs_stack[] = {
    "rax",
    "rbx",
    "rcx",
    "rdx",
};

/*

a + b * c * d

+
 a
 *
  b
  *
   c
   d

d -> ax
c -> bx
mul ax, bx
b -> bx
mul ax, bx
a -> bx
add ax, bx

*/

void codegen_x8664_win_get_val(codegen_x8664_win_info_t *codegen, ast_node_t *node, char *dst)
{
    if (node->type == NT_INT_LIT
     || node->type == NT_STR_LIT
    ) {
        putoutcode("mov %s, %s\n", dst, node->info);
        return;
    } else
    if (node->type == NT_IDENT) {
        codegen_var_info_t *var_info = hashmap_codegen_var_info_t_get(var_offsets, node->info);
        if (!var_info) {
            puts("dfjn");
            exit(1234);
        }
        if (var_info->isstatic) {
            putoutcode("mov %s, [rel %s]\n", dst, node->info);
        } else {
            // printf_s("%s\n", dst);
            putoutcode("mov rax, %s [rbp - %u]\n",
                       codegen_x8664_win_get_asm_type(var_info->type.type),
                       var_info->rbp_offset);
            putoutcode("mov %s, rax\n",
                       dst,
                       codegen_x8664_win_get_asm_type(var_info->type.type),
                       var_info->rbp_offset);
        }
        return;
    }
}

void codegen_x8664_win_eql_expr_gen(codegen_x8664_win_info_t *codegen, ast_node_t *root, size_t reg_num)
{
    if (root->type == NT_IDENT) {
        codegen_var_info_t *var_info = hashmap_codegen_var_info_t_get(var_offsets, root->info);
        if (var_info->isstatic) {
            putoutcode("mov %s, %s\n", expr_regs_stack[reg_num], root->info);
        } else {
            putoutcode("mov %s, rbp - %u\n", expr_regs_stack[reg_num], var_info->rbp_offset);
        }
    } else {
        exit(5);
    }
}

static char *fun_args_regs[] = {
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13"
};

void codegen_x8664_win_expr_gen(codegen_x8664_win_info_t *codegen, ast_node_t *root, size_t reg_num)
{
    if (root->type == NT_IDENT
     || root->type == NT_INT_LIT
     || root->type == NT_STR_LIT) {
        codegen_x8664_win_get_val(codegen, root, expr_regs_stack[reg_num]);
    }
    if (root->type == NT_FUNCTION_CALL) {
        putoutcode("call %s\n", root->childs->val->info);
        putoutcode("mov, %s, [rsp - 16]\n", expr_regs_stack[reg_num]);
    }
    if (root->type == NT_BOP) {
        if (!strcmp(root->info, "=")) {
            if (root->childs->val->type != NT_IDENT) {
                exit(8);
            }
            codegen_x8664_win_eql_expr_gen(codegen, root->childs->val, reg_num);
            codegen_x8664_win_expr_gen(codegen, list_ast_node_t_get(root->childs, 1), reg_num + 1);

            putoutcode("mov [%s], %s\n", expr_regs_stack[reg_num], expr_regs_stack[reg_num + 1]);
            return;
        }
        codegen_x8664_win_expr_gen(codegen, root->childs->val, reg_num);
        codegen_x8664_win_expr_gen(codegen, list_ast_node_t_get(root->childs, 1), reg_num + 1);

        if (!strcmp(root->info, "*")) {
            putoutcode("imul %s, %s\n", expr_regs_stack[reg_num], expr_regs_stack[reg_num + 1]);
        }
        if (!strcmp(root->info, "/")) {
            putoutcode("idiv %s, %s\n", expr_regs_stack[reg_num], expr_regs_stack[reg_num + 1]);
        }
        if (!strcmp(root->info, "+")) {
            putoutcode("add %s, %s\n", expr_regs_stack[reg_num], expr_regs_stack[reg_num + 1]);
        }
        if (!strcmp(root->info, "-")) {
            putoutcode("sub %s, %s\n", expr_regs_stack[reg_num], expr_regs_stack[reg_num + 1]);
        }
    }
}

#endif