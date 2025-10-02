#ifndef ARGS_H
#define ARGS_H

#include <stdio.h>
#include "types.h"

// dst size = MAX_ARG_SIZE;
error_t args_parse_get_tok(char *dst, int argc, char **argv, int *cur_arg)
{
    if (strlen(argv[++(*cur_arg)]) > MAX_ARG_SIZE) {
        return gen_error("too long argument", "", 0, 0);
    }
    strcpy(dst, argv[*cur_arg]);

    return errOK;
}

error_t args_parse(args_t *args, int argc, char **argv)
{
    strcpy(args->infile_name, "");
    strcpy(args->outfile_name, "a.exe");
    strcpy(args->entry_fun_name, "main");
    args->flags = 0;

    for (int cur_arg = 1; cur_arg < argc; cur_arg++) {
        if (argv[cur_arg][0] == '-') {
            if (argv[cur_arg][1] == '\0') {
                return gen_error("expected flag name", args->infile_name, 0, 0);
            }
            if (argv[cur_arg][1] == 'o') {
                error_t err = args_parse_get_tok(args->outfile_name, argc, argv, &cur_arg);
                if (strlen(err.msg)) {
                    printf_s("%s", err.msg);
                    return err;
                }
            } else
            if (argv[cur_arg][1] == 'h') {
                puts(HELP_MENU);
                exit(0);
            } else
            if (argv[cur_arg][1] == 'v') {
                puts(VERSION);
                exit(0);
            } else
            if (argv[cur_arg][1] == 'S') {
                args->flags |= ARGS_FLG_ASM_STOP;
            } else
            if (argv[cur_arg][1] == 'E') {
                args->flags |= ARGS_FLG_PREPROC_STOP;
            } else
            if (argv[cur_arg][1] == 'T') {
                args->flags |= ARGS_FLG_TOKS_PUT;
            } else
            if (argv[cur_arg][1] == 'A') {
                args->flags |= ARGS_FLG_AST_PUT;
            } else
            if (argv[cur_arg][1] == 'c') {
                args->flags |= ARGS_FLG_OBJ_STOP;
            } else
            if (argv[cur_arg][1] == 'e') {
                error_t err = args_parse_get_tok(args->entry_fun_name, argc, argv, &cur_arg);
                if (strlen(err.msg)) {
                    printf_s("%s", err.msg);
                    return err;
                }
            } else {
                return gen_error("unknow flag name", args->infile_name, 0, 0);
            }
        }
        if (cur_arg == 1) {
            strcpy(args->infile_name, argv[1]);
        }
    }

    return errOK;
}

#endif