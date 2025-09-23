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

#include "parser.h"

#include "types/vector.h"

char   *code;
args_t  args;

static vector_error_t_t *err_stk;

void check_errs()
{
    for (int i = 0; i < err_stk->size; i++) {
        put_error(err_stk->arr[i], 0);
    }
    if (err_stk->size) {
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
    if (node->type == NT_BOP)              str = "bop";
    if (node->type == NT_UOP)              str = "uop";
    for (int i = 0; i < tab; i++) {
        putchar(' ');
    }
    printf_s("%s  %s\n", str, node->info);
    size_t i = 0;
    while (i < list_ast_node_t_size(node->childs)) {
        ast_print(list_ast_node_t_get(node->childs, i), tab + 1);
        i++;
    }
}

int main(int argc, char **argv)
{
    err_stk = vector_error_t_create();

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
    vector_token_t_t *toks = vector_token_t_create();
    lexer_create(lexer, args, code, toks, args.infile_name);
    lex_text(lexer);

    for (size_t i = 0; i < toks->size; i++) {
        printf_s("%u:%u %u %s\n", toks->arr[i].line_ref, toks->arr[i].chpos_ref, toks->arr[i].type, toks->arr[i].val);
    }

    lexer_delete(lexer);
    free(lexer);

    vector_token_t_t *my_toks = vector_token_t_create();
    vector_token_t_push_back(my_toks, gen_token(TT_INT_LIT, "5", 0, 0));
    vector_token_t_push_back(my_toks, gen_token(TT_STAR, "*", 0, 0));
    vector_token_t_push_back(my_toks, gen_token(TT_INT_LIT, "2", 0, 0));
    // vector_token_t_push_back(my_toks, gen_token(TT_STAR, "*", 0, 0));
    // vector_token_t_push_back(my_toks, gen_token(TT_INT_LIT, "3", 0, 0));

    parser_info_t *parser = malloc(sizeof(parser_info_t));
    parser_create(parser, args, my_toks, args.infile_name);

    parser_expr_parse(parser, my_toks);

    ast_print(parser->ast_root, 0);

    parser_delete(parser);

    vector_token_t_free(toks);

    check_errs();

    free(err_stk);
    free(code);
    return 0;
}