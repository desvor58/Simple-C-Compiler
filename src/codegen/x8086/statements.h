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


void f(ast_node_t *node, int tab)
{
    char *str = "undef";
    if (node->type == NT_TRANSLATION_UNIT) str = "translation unit";
    if (node->type == NT_BODY)             str = "body";
    if (node->type == NT_VARIABLE_DECL)    str = "var decl";
    if (node->type == NT_EXPR)             str = "expr";
    if (node->type == NT_INT_LIT)          str = "int lit";
    if (node->type == NT_STR_LIT)          str = "str lit";
    if (node->type == NT_IDENT)            str = "ident";
    if (node->type == NT_BOP)              str = "bop";
    if (node->type == NT_UOP)              str = "uop";
    if (node->type == NT_FUNCTION_DECL)    str = "fun decl";
    if (node->type == NT_FUNCTION_CALL)    str = "fun call";
    if (node->type == NT_STMT_RETURN)      str = "stmt return";
    if (node->type == NT_STMT_ASM)         str = "stmt asm";
    if (node->type == NT_STMT_IF)          str = "stmt if";
    if (node->type == NT_STMT_ELSE)        str = "stmt else";

    for (int i = 0; i < tab; i++) {
        putchar(' ');
    }
    printf_s("%s  %s  %u\n", str, (char*)node->info, node->info);
    // if (node->type == NT_FUNCTION_DECL) {
    //     puts("params:");
    //     foreach (list_ast_var_info_t_pair_t,
    //              ((ast_fun_info_t*)node->info)->params
    //     ) {
    //         printf_s("{name:%s type:%u typemodt:%u} ", cur->val->name, cur->val->type.type, cur->val->type.modifires_top);
    //     }
    //     puts("\n");
    // }
    size_t i = 0;
    while (i < list_ast_node_t_size(node->childs)) {
        f(list_ast_node_t_get(node->childs, i), tab + 2);
        i++;
    }
}

void codegen_x8086_stmt_if_gen(codegen_x8086_info_t *codegen)
{
    if (codegen->cur_node->childs->val->type == NT_EXPR) {
        codegen_x8086_expr_gen(codegen,
                               codegen->cur_node->childs->val->childs->val,
                               codegen->namespaces->val->locvar_offset,
                               (ctype_t){CT_INT});
    } else {
        putoutcode("mov %s [bp - %u], %s\n",
                   codegen_x8086_get_asm_type((ctype_t){CT_INT}),
                   codegen->namespaces->val->locvar_offset,
                   codegen->cur_node->childs->val->info);
    }
    putoutcode("cmp %s [bp - %u], 0\n",
               codegen_x8086_get_asm_type((ctype_t){CT_INT}),
               codegen->namespaces->val->locvar_offset);
    if (list_ast_node_t_size(codegen->cur_node->childs) > 2) {
        putoutcode("je L%u\n", codegen->label_count);
        ast_node_t *ast_acc = codegen->cur_node;
        codegen_x8086_namespace_gen(codegen,
                                    codegen->namespaces->val->locvar_offset,
                                    list_ast_node_t_get(codegen->cur_node->childs, 1)->childs->val,
                                    0);
        codegen->cur_node = ast_acc;
        putoutcode("jmp L%u\n", codegen->label_count + 1);
        string_cat(codegen->outcode, "L%u:\n", codegen->label_count);
        codegen_x8086_namespace_gen(codegen,
                                    codegen->namespaces->val->locvar_offset,
                                    list_ast_node_t_get(codegen->cur_node->childs, 2)->childs->val,
                                    0);
        string_cat(codegen->outcode, "L%u:\n", codegen->label_count + 1);
        codegen->label_count += 2;
        return;
    }
    putoutcode("je L%u\n", codegen->label_count);
    codegen_x8086_namespace_gen(codegen,
                                codegen->namespaces->val->locvar_offset,
                                list_ast_node_t_get(codegen->cur_node->childs, 1)->childs->val,
                                0);
    string_cat(codegen->outcode, "L%u:\n", codegen->label_count);
    codegen->label_count++;
}

void codegen_x8086_stmt_asm_gen(codegen_x8086_info_t *codegen)
{
    //printf_s("put this:\n");
    foreach (list_ast_node_t_pair_t, codegen->cur_node->childs) {
        //printf_s("    %s\n", cur->val->info);
        putoutcode("%s\n", cur->val->info);
    }
}

#endif