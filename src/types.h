#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdio.h>

#include "config.h"
#include "list.h"

typedef          char   i8;
typedef          short  i16;
typedef          int    i32;
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef          float  f32;
typedef          double f64;


#define ARGS_FLG_ASM_STOP     1
#define ARGS_FLG_PREPROC_STOP 2
#define ARGS_FLG_TOKS_PUT     4
#define ARGS_FLG_OBJ_STOP     8

typedef struct {
    char infile_name[64];
    char outfile_name[64];
    int  flags;
} args_t;

enum {
    ERROR_CODE_OK,
    ERROR_CODE_EXPECTED_FLAG_NAME,
    ERROR_CODE_UNKNOW_FLAG_NAME,
    ERROR_CODE_WRONG_SYMBOL,
    ERROR_CODE_TOO_LONG_ARGUMENT,
    ERROR_CODE_EXPECTED_INPUT_FILE,
    ERROR_CODE_UNKNOW_PREPROC_DERECTIVE,
    ERROR_CODE_NO_SUCH_INCLUDED_FILE,
    ERROR_CODE_TOO_LARGE_TRANSLATION_UNIT,
    ERROR_CODE_UNKNOW_IDENTIFIRE,
};

typedef struct {
    const char *msg;
    char       *file;
    size_t      line;
    size_t      chpos;
    int         exit_code;
} error_t;

// generate a error
error_t gen_error(const char *msg, char *file, size_t line, size_t chpos, int exit_code)
{
    return (error_t){msg, file, line, chpos, exit_code};
}

#define errOK gen_error("", "", 0, 0, ERROR_CODE_OK)

void put_error(error_t err, int fatal)
{
    printf_s("%s:%u:%u:error: %s\n", err.file, err.line, err.chpos, err.msg);
    if (fatal) {
        exit(err.exit_code);
    }
}


int buf_insert(char *buf, size_t buf_size, size_t index, char *insert)
{
    size_t buflen = strlen(buf);
    size_t insertlen = strlen(insert);
    
    if (buf_size < buflen + insertlen
     || index >= buflen) {
        return 1;
    }

    char *save_buf = (char*)malloc(buf_size);

    size_t i;
    for (i = index; i < buflen; i++) {
        save_buf[i - index] = buf[i];
    }
    save_buf[i] = '\0';
    for (i = 0; i < insertlen; i++) {
        buf[i + index] = insert[i];
    }
    for (i = 0; i < strlen(save_buf); i++) {
        buf[i + index + insertlen] = save_buf[i];
    }
    buf[i] = '\0';

    free(save_buf);

    return 0;
}

// create a new string in heap
// delete part of str and replace to what
int buf_replace(char *buf, size_t buf_size, u32 repl_start, u32 repl_end, char *repl)
{
    size_t buflen = strlen(buf);
    size_t repllen = strlen(repl);

    if (repl_start > repl_end
     || repl_end > buflen
     || buf_size < (buflen - (repl_end - repl_start)) + repllen) {
        return 1;
    }

    char *save_buf = (char*)malloc((buflen - (repl_end - repl_start)) + repllen);
    size_t i = repl_end;
    while (i < buflen) {
        save_buf[i - repl_end] = buf[i];
        i++;
    }
    save_buf[i - repl_end] = '\0';
    for (i = 0; i < repllen; i++) {
        buf[repl_start + i] = repl[i];
    }
    for (i = 0; i < strlen(save_buf); i++) {
        buf[i + repl_start + repllen] = save_buf[i];
    }
    buf[i + repl_start + repllen] = '\0';

    free(save_buf);
    return 0;
}

void put_pos_str(char *str, size_t pos)
{
    for (size_t i = 0; i < strlen(str); i++) {
        if (i == pos) {
            printf_s("$");
        }
        putchar(str[i]);
    }
    putchar('\n');
}

// ERROR
void get_folder_path(char *path, char *buf)
{
    long long top = strlen(buf) + 1;
    while (path[top] != '/' && top >= 0) {
        top--;
    }
    buf[0] = '.';
    buf[1] = '/';
    size_t i = 2;
    while (i - 2 <= top) {
        buf[i++] = path[i - 2];
    }
    buf[i] = '\0';
}


typedef struct {
    char name[MAX_IDENT_SIZE];
    char args[MAX_MACRO_ARGS][MAX_IDENT_SIZE];
    char val[MAX_MACRO_VAL];
} macro_info_t;

#endif