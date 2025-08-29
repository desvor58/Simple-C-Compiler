#include "args.h"
#include "preproc.h"

char   *code;
args_t  args;
error_t errs[ERROR_STK_SIZE];
u32     errs_top = 0;

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
    }
    code[i] = '\0';

    preproc(args, errs, &errs_top, code, args.infile_name);
    if (args.flags & ARGS_FLG_PREPROC_STOP) {
        printf_s("%s\n", code);
    }

    return 0;
}