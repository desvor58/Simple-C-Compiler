#ifndef CODEGEN_X8664_WIN
#define CODEGEN_X8664_WIN

#include "../types.h"
#include "../types/hashmap.h"

typedef struct {
    size_t locvar_offset;
} codegen_namespace_info_t;

genlist(codegen_namespace_info_t);

typedef struct {
    args_t                                args;
    ast_node_t                           *ast_root;
    ast_node_t                           *cur_node;
    string_t                             *outcode;
    size_t                                outcode_offset;
    list_codegen_namespace_info_t_pair_t *namespaces;
} codegen_x8664_win_info_t;

codegen_x8664_win_info_t *codegen_x8664_win_create(args_t args, ast_node_t *ast)
{
    codegen_x8664_win_info_t *codegen = (codegen_x8664_win_info_t*)malloc(sizeof(codegen_x8664_win_info_t));
    codegen->args = args;
    codegen->ast_root = ast;
    codegen->cur_node = ast;
    codegen->outcode = string_create();
    codegen->outcode_offset = 0;
    codegen->namespaces = list_codegen_namespace_info_t_create();
    codegen_namespace_info_t *global_namespace = malloc(sizeof(codegen_namespace_info_t));
    global_namespace->locvar_offset = 0;
    list_codegen_namespace_info_t_add(codegen->namespaces, global_namespace);
    return codegen;
}

void codegen_x8664_win_delete(codegen_x8664_win_info_t *codegen)
{
    foreach (list_codegen_namespace_info_t_pair_t, codegen->namespaces) {
        free(cur->val);
    }
    list_codegen_namespace_info_t_free(codegen->namespaces);
    string_free(codegen->outcode);
}

static vector_error_t_t *err_stk;

typedef struct {
    size_t  rbp_offset;
    ctype_t type;
    int     isstatic;
} codegen_var_info_t;

genhashmap(codegen_var_info_t)

hashmap_codegen_var_info_t_t *var_offsets;

void codegen_x8664_win_fun_decl(codegen_x8664_win_info_t *codegen);
void codegen_x8664_win_static_var_decl(codegen_x8664_win_info_t *codegen);

#define putoutcode(fmt, ...)  \
    for (size_t outcode_offset_counter = 0; outcode_offset_counter < codegen->outcode_offset; outcode_offset_counter++)  \
        string_cat(codegen->outcode, "    ");  \
    string_cat(codegen->outcode, fmt, __VA_ARGS__)

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

size_t codegen_x8664_win_get_type_size(ctype_type ctype)
{
    if (ctype == CT_CHAR) {
        return 1;
    } else
    if (ctype == CT_SHORT) {
        return 2;
    } else
    if (ctype == CT_INT) {
        return 4;
    } else
    if (ctype == CT_LONG) {
        return 8;
    }
    return 0;
}

char *codegen_x8664_win_get_static_asm_type(ctype_type ctype)
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

char *codegen_x8664_win_get_asm_type(ctype_type ctype)
{
    if (ctype == CT_CHAR) {
        return "byte";
    } else
    if (ctype == CT_SHORT) {
        return "word";
    } else
    if (ctype == CT_INT) {
        return "dword";
    } else
    if (ctype == CT_LONG) {
        return "qword";
    }
}

static char *expr_regs_stack[] = {
    "rax",
    "rbx",
    "rcx",
    "rdx",
};

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

void codegen_x8664_win_expr_gen(codegen_x8664_win_info_t *codegen, ast_node_t *root, size_t reg_num)
{
    if (root->type == NT_IDENT
     || root->type == NT_INT_LIT
     || root->type == NT_STR_LIT) {
        codegen_x8664_win_get_val(codegen, root, expr_regs_stack[reg_num]);
    }
    if (root->type == NT_FUNCTION_CALL) {
        if (list_ast_node_t_size(root->childs) == 1) {
            putoutcode("call %s\n", root->childs->val->info);
        }
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
    string_t *dst_str = string_create();
    string_cat(dst_str, "%s [rbp - %u]",
                        codegen_x8664_win_get_asm_type(var_info->type.type),
                        codegen->namespaces->val->locvar_offset += codegen_x8664_win_get_type_size(var_info->type.type));
    string_push_back(dst_str, '\0');
    codegen_x8664_win_get_val(codegen, codegen->cur_node->childs->val, dst_str->str);
    string_free(dst_str);
}

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

static char *fun_args_regs[] = {
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13"
};

void codegen_x8664_win_fun_decl(codegen_x8664_win_info_t *codegen)
{
    ast_fun_info_t *fun_info = (ast_fun_info_t*)codegen->cur_node->info;
    if (!fun_info) {
        vector_error_t_push_back(err_stk, gen_error("fun declaration node havent ast_fun_info", codegen->args.infile_name, 0, 0));
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
        vector_error_t_push_back(err_stk, gen_error("var declaration node havent ast_var_info", codegen->args.infile_name, 0, 0));
        return;
    }
    putoutcode("%s %s %s\n",
               var_info->name,
               codegen_x8664_win_get_static_asm_type(var_info->type.type),
               (char*)codegen->cur_node->childs->val->info);
}

#endif