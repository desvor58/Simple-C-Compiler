#ifndef CODEGEN_X8664_WIN
#define CODEGEN_X8664_WIN

#include "../types.h"
#include "../types/hashmap.h"

genhashmap(size_t);

typedef struct {
    args_t      args;
    ast_node_t *ast_root;
    ast_node_t *cur_node;
    string_t   *outcode;
    size_t      locvar_offset;
} codegen_x8664_win_info_t;

codegen_x8664_win_info_t *codegen_x8664_win_create(args_t args, ast_node_t *ast)
{
    codegen_x8664_win_info_t *codegen = (codegen_x8664_win_info_t*)malloc(sizeof(codegen_x8664_win_info_t));
    codegen->args = args;
    codegen->ast_root = ast;
    codegen->cur_node = ast;
    codegen->outcode = string_create();
    codegen->locvar_offset = 0;
    return codegen;
}

void codegen_x8664_win_delete(codegen_x8664_win_info_t *codegen)
{
    string_free(codegen->outcode);
}

static vector_error_t_t *err_stk;

hashmap_size_t_t *var_offsets;

void codegen_x8664_win_fun_decl(codegen_x8664_win_info_t *codegen);
void codegen_x8664_win_static_var_decl(codegen_x8664_win_info_t *codegen);

void codegen_x8664_win(codegen_x8664_win_info_t *codegen)
{
    var_offsets = hashmap_size_t_create();

    string_cat(codegen->outcode, "global ");
    string_cat(codegen->outcode, codegen->args.entry_fun_name);
    string_cat(codegen->outcode, "\n");

    string_cat(codegen->outcode, "section .data\n");
    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_VARIABLE_DECL) {
            codegen_x8664_win_static_var_decl(codegen);
        }
    }

    string_cat(codegen->outcode, "section .text\n");
    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_FUNCTION_DECL) {
            codegen_x8664_win_fun_decl(codegen);
        }
    }
}

const char *codegen_x8664_win_get_type_size(ctype_type ctype)
{
    if (ctype == CT_CHAR) {
        return "1";
    } else
    if (ctype == CT_SHORT) {
        return "2";
    } else
    if (ctype == CT_INT) {
        return "4";
    } else
    if (ctype == CT_LONG) {
        return "8";
    }
}

const char *codegen_x8664_win_get_asm_type(ctype_type ctype)
{
    if (ctype == CT_CHAR) {
        return "db";
    } else
    if (ctype == CT_SHORT) {
        return "dw";
    } else
    if (ctype == CT_INT) {
        return "dd";
    } else
    if (ctype == CT_LONG) {
        return "dq";
    }
}

void codegen_x8664_win_var_decl(codegen_x8664_win_info_t *codegen)
{
    ast_var_info_t *var_info = codegen->cur_node->info;

}

void codegen_x8664_win_namespace_gen(codegen_x8664_win_info_t *codegen)
{
    foreach (list_ast_node_t_pair_t, codegen->ast_root->childs) {
        codegen->cur_node = cur->val;
        if (cur->val->type == NT_VARIABLE_DECL) {
            codegen_x8664_win_var_decl(codegen);
        }
    }
}

void codegen_x8664_win_fun_decl(codegen_x8664_win_info_t *codegen)
{
    ast_fun_info_t *fun_info = (ast_fun_info_t*)codegen->cur_node->info;
    if (!fun_info) {
        vector_error_t_push_back(err_stk, gen_error("fun declaration node havent ast_fun_info", codegen->args.infile_name, 0, 0));
        return;
    }
    string_cat(codegen->outcode, fun_info->name);
    string_cat(codegen->outcode, ":");
    string_cat(codegen->outcode, "push rbp\n"
                                 "mov rbp, rsp\n");

    // TODO: namespace codegen

    string_cat(codegen->outcode, "pop rbp\n"
                                 "ret\n");
}

void codegen_x8664_win_static_var_decl(codegen_x8664_win_info_t *codegen)
{
    ast_var_info_t *var_info = (ast_var_info_t*)codegen->cur_node->info;
    if (!var_info) {
        vector_error_t_push_back(err_stk, gen_error("var declaration node havent ast_var_info", codegen->args.infile_name, 0, 0));
        return;
    }
    string_cat(codegen->outcode, var_info->name);
    string_cat(codegen->outcode, " ");
    string_cat(codegen->outcode, codegen_x8664_win_get_asm_type(var_info->type.type));
    string_cat(codegen->outcode, " ");
    string_cat(codegen->outcode, (char*)codegen->cur_node->childs[0].val->info);
    string_cat(codegen->outcode, "\n");
}

#endif