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
#include "tokenizer.h"
#include "lexer.h"
#include "parser.h"
#include "codegen/x8664_win.h"
#include "codegen/x8086.h"

string_t *code;
args_t    args;

static list_error_t_pair_t *err_stk;

void check_errs()
{
    for (int i = 0; i < list_error_t_size(err_stk); i++) {
        put_error(*list_error_t_get(err_stk, i), 0);
    }
    if (list_error_t_size(err_stk)) {
        exit(1);
    }
}

void ast_print(ast_node_t *node, int tab)
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
        ast_print(list_ast_node_t_get(node->childs, i), tab + 2);
        i++;
    }
}

int main(int argc, char **argv)
{
    err_stk = list_error_t_create();

    code = string_create("");

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
    //printf_s("%s\nEND\n", code->str);
    fclose(infile);

    /* --- preprocess --- */
        preproc_info_t *preproc = preproc_create(err_stk, args, code, args.infile_name);
        preprocess(preproc);
        preproc_delete(preproc);
        if (args.flags & ARGS_FLG_PREPROC_STOP) {
            printf_s("%s\n", code->str);
        }
    /* ------------------ */

    /* --- tokenization --- */
        vector_token_t_t *toks = vector_token_t_create();
        tokenizer_info_t *tokenizer = tokenizer_create(args, code->str, toks, args.infile_name);
        tokenize_text(tokenizer);
        if (args.flags & ARGS_FLG_TOKS_PUT) {
            for (size_t i = 0; i < toks->size; i++) {
                printf_s("%u. %s:%u:%u %u %s\n",
                         i,
                         toks->arr[i].file_ref,
                         toks->arr[i].line_ref,
                         toks->arr[i].chpos_ref,
                         toks->arr[i].type,
                         toks->arr[i].val);
            }
        }

        tokenizer_delete(tokenizer);
    /* -------------------- */

    /* --- lexical analis --- */
        lexer_info_t *lexer = lexer_create(err_stk, args, toks);
        lex_text(lexer);
        check_errs();
        lexer_delete(lexer);
    /* ---------------------- */

    /* --- parsing --- */
        parser_info_t *parser = parser_create(err_stk, args, toks, args.infile_name);
        parse(parser);
        if (args.flags & ARGS_FLG_AST_PUT) {
            ast_print(parser->ast_root, 0);
        }

        /* --- codegen --- */
            if (args.target == TRGT_x8664_WIN) {
                codegen_x8664_win_info_t *codegen = codegen_x8664_win_create(err_stk, args, parser->ast_root);

                codegen_x8664_win(codegen);
                if (args.flags & ARGS_FLG_ASM_STOP) {
                    FILE *sfile;
                    fopen_s(&sfile, "a.s", "w");
                    fprintf_s(sfile, "%s", codegen->outcode->str);
                    fclose(sfile);
                }
                
                codegen_x8664_win_delete(codegen);
            } else
            if (args.target == TRGT_x8086) {
                codegen_x8086_info_t *codegen = codegen_x8086_create(err_stk, args, parser->ast_root);

                codegen_x8086(codegen);
                if (args.flags & ARGS_FLG_ASM_STOP) {
                    FILE *sfile;
                    fopen_s(&sfile, "a.s", "w");
                    fprintf_s(sfile, "%s", codegen->outcode->str);
                    fclose(sfile);
                }
                
                codegen_x8086_delete(codegen);
            }
        /* --------------- */

        parser_delete(parser);
    /* --------------- */

    vector_token_t_free(toks);


    check_errs();

    list_error_t_free(err_stk);
    string_free(code);
    return 0;
}