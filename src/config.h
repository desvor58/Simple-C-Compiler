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

#define VERSION "SCC by Desvor v0.0.69"
#define HELP_MENU                                                                                  \
    "usage: scc [options] <C code file name>"                                                      \
    "options:\n"                                                                                   \
    "    -E                  Put translation unit after preprocessing\n"                           \
    "    -S                  Put translation unit after code generation and put it to a.s file\n"  \
    "    -T                  Put the tokens\n"                                                     \
    "    -A                  Put AST\n"                                                            \
    "    -c                  Stop compiling after assembling and do not link\n"                    \
    "    -o <file>           Put the output into <file>\n"                                         \
    "    -e <function name>  Set entry function to <function name>"                                \
    "    -v                  Put version of prcc\n"                                                \

#endif