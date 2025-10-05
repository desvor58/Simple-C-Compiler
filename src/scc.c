/**********************************************************
 *                 Simple C Compiler
 *                     by Desvor
 *     (thanks halloweeks for crc32 hash-fuction)
 *
 *  This compiler created for the OS x16PRox (by PRoxDev),
 * for creating your own lightweight compilers on this base
 *           and for learn who C compilers works
 **********************************************************/

#include "args.h"
#include "preproc.h"
#include "lexer.h"
#include "parser.h"
#include "codegen/codegen_x8664_win.h"

string_t *code;
args_t    args;

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
    char *str = "undef";
    if (node->type == NT_TRANSLATION_UNIT) str = "translation unit";
    if (node->type == NT_VARIABLE_DECL)    str = "var decl";
    if (node->type == NT_EXPR)             str = "expr";
    if (node->type == NT_INT_LIT)          str = "int lit";
    if (node->type == NT_STR_LIT)          str = "str lit";
    if (node->type == NT_IDENT)            str = "ident";
    if (node->type == NT_BOP)              str = "bop";
    if (node->type == NT_UOP)              str = "uop";
    if (node->type == NT_FUNCTION_DECL)    str = "fun decl";

    for (int i = 0; i < tab; i++) {
        putchar(' ');
    }
    printf_s("%s  %s  %u\n", str, (char*)node->info, node->info);
    size_t i = 0;
    while (i < list_ast_node_t_size(node->childs)) {
        ast_print(list_ast_node_t_get(node->childs, i), tab + 1);
        i++;
    }
}

int main(int argc, char **argv)
{
    err_stk = vector_error_t_create();

    code = string_create();

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

    /* --- preprocess --- */
        preproc_info_t *preproc = preproc_create(args, code->str, args.infile_name);
        preprocess(preproc);
        preproc_delete(preproc);
        if (args.flags & ARGS_FLG_PREPROC_STOP) {
            printf_s("%s\n", code->str);
        }
    /* ------------------ */

    /* --- lexical analis / tokenization --- */
        vector_token_t_t *toks = vector_token_t_create();
        lexer_info_t *lexer = lexer_create(args, code->str, toks, args.infile_name);
        lex_text(lexer);
        if (args.flags & ARGS_FLG_TOKS_PUT) {
            for (size_t i = 0; i < toks->size; i++) {
                printf_s("%u:%u %u %s\n", toks->arr[i].line_ref, toks->arr[i].chpos_ref, toks->arr[i].type, toks->arr[i].val);
            }
        }

        lexer_delete(lexer);
    /* ------------------------------------- */

    /* --- parsing --- */
        parser_info_t *parser = parser_create(args, toks, args.infile_name);
        parse(parser);
        if (args.flags & ARGS_FLG_AST_PUT) {
            ast_print(parser->ast_root, 0);
        }

        /* --- codegen --- */
            codegen_x8664_win_info_t *codegen = codegen_x8664_win_create(args, parser->ast_root);

            codegen_x8664_win(codegen);
            if (args.flags & ARGS_FLG_ASM_STOP) {
                printf_s("%s\n", codegen->outcode->str);
            }
            
            codegen_x8664_win_delete(codegen);
        /* --------------- */

        parser_delete(parser);
    /* --------------- */

    vector_token_t_free(toks);

    check_errs();

    free(err_stk);
    string_free(code);
    return 0;
}