/**********************************************************
 *                 Simple C Compiler
 *                     by Desvor
 *     (thanks halloweeks for crc32 hash-fuction)
 *
 * This compiler created for the OS x16PRox (by PRoxDev),
 * for creating your own lightweight compilers on this base
 *           and for learn who C compilers works
 **********************************************************/

#include "args.h"
#include "preproc.h"
#include "lexer.h"

#include "hashmap.h"

char   *code;
args_t  args;
static error_stk_t *err_stk;

void check_errs()
{
    for (int i = 0; i < err_stk->top; i++) {
        put_error(err_stk->stk[i], 0);
    }
    if (err_stk->top) {
        exit(1);
    }
}

void ast_print(ast_node_t *node, int tab)
{
    char *str = "";
    if (node->type == NT_TRANSLATION_UNIT) str = "translation unit";
    if (node->type == NT_EXPR)             str = "expr";
    if (node->type == NT_INT_LIT)          str = "int";
    if (node->type == NT_STR_LIT)          str = "str";
    for (int i = 0; i < tab; i++) {
        putchar(' ');
    }
    printf_s("%s\n", str);
    size_t i = 0;
    while (i < list_ast_node_t_size(node->childs)) {
        ast_print(list_ast_node_t_get(node->childs, i), tab + 1);
        i++;
    }
}

genlist(int)

int main(int argc, char **argv)
{
    int v1 = 5;
    int v2 = 7;
    int v3 = 1;

    list_int_pair_t *list = list_int_create();
    list_int_add(list, &v1);
    list_int_add(list, &v2);
    list_int_add(list, &v3);

    foreach(list_int_pair_t, list) {
        printf_s("%d\n", *cur->val);
    }

    return 0;

    err_stk = (error_stk_t*)malloc(sizeof(error_stk_t));
    err_stk->top = 0;

    code = (char*)malloc(MAX_CODE_SIZE);

    error_t err = args_parse(&args, argc, argv);
    if (strlen(err.msg)) {
        put_error(err, 1);
    }
    
    if (!strcmp(args.infile_name, "")) {
        put_error(gen_error("expected input file name", "", 0, 0), 1);
    }
    FILE *infile;
    fopen_s(&infile, args.infile_name, "r");
    if (!infile) {
        put_error(gen_error("input file not be opened", "", 0, 0), 1);
    }
    get_file_text(infile, code);
    fclose(infile);

    preproc_info_t *preproc = malloc(sizeof(preproc_info_t));
    preproc_create(preproc, args, code, args.infile_name);
    preprocess(preproc);
    preproc_delete(preproc);
    free(preproc);
    if (args.flags & ARGS_FLG_PREPROC_STOP) {
        printf_s("%s\n", code);
    }

    lexer_info_t *lexer = malloc(sizeof(lexer_info_t));
    token_t *toks = malloc(sizeof(token_t)*TOKS_SIZE);
    size_t toks_top = 0;
    lexer_create(lexer, args, code, toks, &toks_top, args.infile_name);
    lex_text(lexer);

    for (size_t i = 0; i < toks_top; i++) {
        printf_s("%u:%u %u %s\n", toks[i].line_ref, toks[i].chpos_ref, toks[i].type, toks[i].val);
    }

    lexer_delete(lexer);
    free(lexer);

    check_errs();

    free(err_stk);
    free(code);
    return 0;
}