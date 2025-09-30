#ifndef CODEGEN_X8664_WIN
#define CODEGEN_X8664_WIN

#include "types.h"

typedef struct {
    args_t            args;
    ast_node_t       *ast_root;
    ast_node_t       *cur_node;
} codegen_x8664_win_info_t;

codegen_x8664_win_info_t *codegen_x8664_win_create(args_t args, ast_node_t *ast)
{
    codegen_x8664_win_info_t *codegen = (codegen_x8664_win_info_t*)malloc(sizeof(codegen_x8664_win_info_t));
    codegen->args = args;
    codegen->ast_root = ast;
    codegen->cur_node = ast;
    return codegen;
}

void codegen_x8664_win_delete(codegen_x8664_win_info_t *codegen)
{
}



#endif