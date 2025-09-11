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
error_t errs[ERROR_STK_SIZE];
u32     errs_top = 0;

genlist(int)

int main(int argc, char **argv)
{
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
    char c = ' ';
    int i = 0;
    for (; c != EOF; i++) {
        c = getc(infile);
        code[i] = c;
        if (c == '/') {
            c = getc(infile);
            if (c == '/') {
                while (c != '\n' && c != EOF) {
                    c = getc(infile);
                }
                code[i--] = '\n';
            }
        }
    }
    code[i] = '\0';

    preprocess(args, errs, &errs_top, code, args.infile_name);
    if (args.flags & ARGS_FLG_PREPROC_STOP) {
        printf_s("%s\n", code);
    }

    return 0;
}