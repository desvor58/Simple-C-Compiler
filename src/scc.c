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

int main(int argc, char **argv)
{
    err_stk = (error_stk_t*)malloc(sizeof(error_stk_t));
    err_stk->top = 0;

    code = (char*)malloc(MAX_CODE_SIZE);

    error_t err = args_parse(&args, argc, argv);
    if (err.exit_code) {
        put_error(err, 1);
    }
    
    if (!strcmp(args.infile_name, "")) {
        put_error(gen_error("expected input file name", "", 0, 0, ERROR_CODE_EXPECTED_INPUT_FILE), 1);
    }
    FILE *infile;
    fopen_s(&infile, args.infile_name, "r");
    if (!infile) {
        put_error(gen_error("input file not be opened", "", 0, 0, 1488), 1);
    }
    get_file_text(infile, code);
    fclose(infile);

    preprocess(args, code, args.infile_name);
    check_errs();
    if (args.flags & ARGS_FLG_PREPROC_STOP) {
        printf_s("%s\n", code);
    }

    free(err_stk);
    free(code);
    return 0;
}