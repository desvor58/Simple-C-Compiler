#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ARG_SIZE         64
#define MAX_IDENT_SIZE       64
#define MAX_MACRO_ARGS       8
#define MAX_FUNCS_ARGS       8
#define MAX_CODE_SIZE        1024 * 32
#define MAX_MACRO_VAL        1024 * 4
#define HASHMAP_BUCKIT_NUM   64
#define MAX_HASHMAP_KEYS_BUF 1024

#define VERSION "PRCC by Desvor v1.0.0\n"
#define HELP_MENU                                                                            \
    "usage: scc [options] <C code file name>"                                                \
    "options:\n"                                                                             \
    "    -E         Stop compiling after preprocessing and put translation unit\n"           \
    "    -S         Stop compiling after code-generation and write to a.s generated code\n"  \
    "    -T         Stop compiling after tokenization and put the tokens\n"                  \
    "    -c         Stop compiling after assembling and do not link\n"                       \
    "    -o <file>  Put the output into <file>\n"                                            \
    "    -v         Put version of prcc\n"                                                   \

#endif