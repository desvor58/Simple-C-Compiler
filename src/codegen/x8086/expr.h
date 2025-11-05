#ifndef CODEGNE_X8086_EXPR_H
#define CODEGNE_X8086_EXPR_H

#include "common.h"

void codegen_x8086_cast(codegen_x8086_info_t *codegen, codegen_var_info_t *var_info, ctype_t cast_type, char *dst)
{
    if (var_info->type.type == CT_CHAR && (cast_type.type == CT_SHORT || cast_type.type == CT_INT)) {
        putoutcode("movsx %s, byte [bp - %u]\n", dst, var_info->rbp_offset);
    } else
    if ((var_info->type.type == CT_SHORT || var_info->type.type == CT_INT) && cast_type.type == CT_CHAR) {
        putoutcode("mov %s, word [bp - %u]\n", dst, var_info->rbp_offset);
    }
}

void codegen_x8086_get_val(codegen_x8086_info_t *codegen, ast_node_t *node, size_t dst_offset, ctype_t expected_type)
{
    if (node->type == NT_INT_LIT
     || node->type == NT_FLOAT_LIT
    ) {
        putoutcode("mov %s [bp - %u], %s\n",
                   codegen_x8086_get_asm_type(expected_type),
                   dst_offset,
                   node->info);
    } else
    if (node->type == NT_STR_LIT) {
        string_t *str = string_create();
        string_cat(str, "%s", node->info);
        string_push_back(str, '\0');
        list_string_t_add(codegen->data_str_lits, str);
        putoutcode("mov %s [bp - %u], %s\n",
                   codegen_x8086_get_asm_type(expected_type),
                   dst_offset,
                   format("strlit%u", codegen->data_str_lit_count++));
    } else
    if (node->type == NT_IDENT) {
        codegen_var_info_t *var_info = hashmap_codegen_var_info_t_get(var_offsets, node->info);
        if (!var_info) {
            puts("error in x8086:expr:get_val:if ident");
            exit(1);
        }
        if (var_info->isstatic) {
            putoutcode("mov %s [bp - %u], [rel %s]\n",
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset,
                       node->info);
        } else {
            char *reg = alloca(3);
            codegen_x8086_get_reg(reg, 'a', expected_type);
            if (var_info->type.type != expected_type.type) {
                codegen_x8086_cast(codegen, var_info, expected_type, "ax");
            } else {
                putoutcode("mov ax, [bp - %u]\n", var_info->rbp_offset);
            }
            putoutcode("mov %s [bp - %u], %s\n",
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset,
                       reg);
        }
    }
}

void codegen_x8086_eql_expr_gen(codegen_x8086_info_t *codegen, ast_node_t *root, char *dst_reg)
{
    codegen_var_info_t *var_info = hashmap_codegen_var_info_t_get(var_offsets, root->info);
    if (var_info->isstatic) {
        putoutcode("mov %s, %s\n",
                   dst_reg,
                   root->info);
    } else {
        putoutcode("sub bp, %u\n",
                   var_info->rbp_offset);
        putoutcode("mov %s, bp\n",
                   dst_reg);
        putoutcode("add bp, %u\n",
                   var_info->rbp_offset);
    }
}

void codegen_x8086_expr_gen(codegen_x8086_info_t *codegen, ast_node_t *root, size_t dst_offset, ctype_t expected_type)
{
    if (root->type == NT_INT_LIT
     || root->type == NT_FLOAT_LIT
     || root->type == NT_STR_LIT
     || root->type == NT_IDENT
    ) {
        codegen_x8086_get_val(codegen, root, dst_offset, expected_type);
    } else
    if (root->type == NT_FUNCTION_CALL) {
        ast_fun_info_t *fun_info = root->info;
        putoutcode("sub sp, %u\n", codegen->namespaces->val->locvar_offset - 2);
        if (list_ast_node_t_size(root->childs) > 1) {
            size_t offset = 4;
            size_t i = 0;
            foreach (list_ast_node_t_pair_t, root->childs->next) {
                codegen_x8086_expr_gen(codegen, cur->val, offset, list_ast_var_info_t_get(fun_info->params, i)->type);
                offset += codegen_x8086_get_type_size(list_ast_var_info_t_get(fun_info->params, i)->type);
                i++;
            }
        }
        putoutcode("call %s\n", root->childs->val->info);
        putoutcode("add sp, %u\n", codegen->namespaces->val->locvar_offset - 2);
        putoutcode("mov %s [bp - %u], ax\n", codegen_x8086_get_asm_type(expected_type), dst_offset);
    } else
    if (root->type == NT_BOP) {
        if (!strcmp(root->info, "=")) {
            if (root->childs->val->type != NT_IDENT) {
                puts("error in x8086:expr:expr_gen:if bop=");
                exit(1);
            }
            codegen_var_info_t *var_info = hashmap_codegen_var_info_t_get(var_offsets, root->childs->val->info);
            codegen_x8086_eql_expr_gen(codegen, root->childs->val, "bx");
            codegen_x8086_expr_gen(codegen,
                                   list_ast_node_t_get(root->childs, 1),
                                   dst_offset + codegen_x8086_get_type_size(expected_type),
                                   var_info->type);
            char *reg = alloca(3);
            codegen_x8086_get_reg(reg, 'a', var_info->type);
            putoutcode("mov %s, [bp - %u]\n",
                       reg,
                       dst_offset + codegen_x8086_get_type_size(expected_type));
            putoutcode("mov [bx], %s\n", reg);
            return;
        }
        codegen_x8086_expr_gen(codegen,
                               root->childs->val,
                               dst_offset,
                               expected_type);
        codegen_x8086_expr_gen(codegen,
                               list_ast_node_t_get(root->childs, 1),
                               dst_offset + codegen_x8086_get_type_size(expected_type),
                               expected_type);
        char *reg = alloca(sizeof(char)*3);
        codegen_x8086_get_reg(reg, 'a', expected_type);
        putoutcode("mov %s, [bp - %u]\n",
                    reg,
                    dst_offset);
        if (!strcmp(root->info, "*")) {
            putoutcode("imul %s, %s [bp - %u]\n",
                       reg,
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset + codegen_x8086_get_type_size(expected_type));
        } else
        if (!strcmp(root->info, "/")) {
            putoutcode("imul %s, %s [bp - %u]\n",
                       reg,
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset + codegen_x8086_get_type_size(expected_type));
        } else
        if (!strcmp(root->info, "+")) {
            putoutcode("add %s, %s [bp - %u]\n",
                       reg,
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset + codegen_x8086_get_type_size(expected_type));
        } else
        if (!strcmp(root->info, "-")) {
            putoutcode("sub %s, %s [bp - %u]\n",
                       reg,
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset + codegen_x8086_get_type_size(expected_type));
        }
        putoutcode("mov %s [bp - %u], %s\n",
                    codegen_x8086_get_asm_type(expected_type),
                    dst_offset,
                    reg);
    } else
    if (root->type == NT_UOP) {
        if (!strcmp(root->info, "&")) {
            if (root->childs->val->type != NT_IDENT) {
                puts("codegen fatal error: expected ident after &");
                exit(1);
            }
            putoutcode("mov %s [bp - %u], bp - %u\n",
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset,
                       hashmap_codegen_var_info_t_get(var_offsets, root->childs->val->info)->rbp_offset);
            return;
        }
        codegen_x8086_expr_gen(codegen,
                               root->childs->val,
                               dst_offset,
                               expected_type);
        if (!strcmp(root->info, "-")) {
            putoutcode("neg %s [bp - %u]\n",
                       codegen_x8086_get_asm_type(expected_type),
                       dst_offset);
        }
    }
}

#endif