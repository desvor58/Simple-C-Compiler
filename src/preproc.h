#ifndef PREPROC_H
#define PREPROC_H

#include "types.h"

void __preproc_skip_notoks(char *text, size_t *pos)
{
    while (!isalpha(text[*pos]) && text[*pos] != '_' && text[*pos] != '#') {
        (*pos)++;
    }
}

void __preproc_gettok(char *text, size_t *pos, char *buf)
{
    int i = 0;
    while (isalpha(text[*pos]) || isdigit(text[*pos]) || text[*pos] == '_' || text[*pos] == '#') {
        buf[i] = text[*pos];
        i++;
        (*pos)++;
    }
    buf[i] = '\0';
}

// errs size = ERROR_STK_SIZE
void preproc(args_t args, error_t *errs, u32 *errs_top, char *text, char *file_name)
{
    size_t pos;
    size_t line = 1;
    size_t chpos = 0;
    char  *file = file_name;

    char *buf = malloc(MAX_IDENT_SIZE);

    for (pos = 0; pos < strlen(text); pos++) {
        if (text[pos] == '\n') {
            line++;
            chpos = 1;
        } else {
            chpos++;
        }
        if (text[pos] == '/' && text[pos + 1] == '/') {
            size_t start_pos = pos;
            while (text[pos] != '\n' && text[pos] != '\0') {
                pos++;
            }
            buf_replace(text, 1024 * 10, start_pos, pos, "");
            pos = start_pos;
        } else
        if (text[pos] == '#') {
            u32 start_pos = pos;
            __preproc_gettok(text, &pos, buf);

            if (!strcmp(buf, "#include")) {
                while (text[pos] == ' ') {
                    pos++;
                }
                if (text[pos] == '"') {
                    int i;
                    buf[0] = '\0';
                    for (i = 0; text[++pos] != '"'; i++) {
                        buf[i] = text[pos];
                    }
                    buf[i] = '\0';
                    char *full_path = malloc(128);
                    get_folder_path(file, full_path);
                    strcat_s(full_path, 128, buf);

                    FILE *included_file;
                    fopen_s(&included_file, full_path, "r");
                    free(full_path);
                    if (!included_file) {
                        errs[(*errs_top)++] = gen_error("No such included file", file, line, chpos, ERROR_CODE_NO_SUCH_INCLUDED_FILE);
                    } else {
                        char *included_code = (char*)malloc(MAX_CODE_SIZE);
                        char c = ' ';
                        int i = 0;
                        for (; c != EOF; i++) {
                            c = getc(included_file);
                            included_code[i] = c;
                        }
                        included_code[i] = '\0';

                        preproc(args, errs, errs_top, included_code, buf);
                        //printf_s("%s\n", included_code);

                        int err = buf_replace(text, MAX_CODE_SIZE, start_pos, pos + 1, included_code);
                        if (err) {
                            put_error(gen_error("too large included file (translation unit > 1024 * 10)", file, line, chpos, ERROR_CODE_TOO_LARGE_TRANSLATION_UNIT), 1);
                        }

                        free(included_code);
                    }
                }
            } else {
                errs[(*errs_top)++] = gen_error("Unknow preprocessor derective",
                                                file,
                                                line,
                                                chpos,
                                                ERROR_CODE_UNKNOW_PREPROC_DERECTIVE);
            }
        }
    }

    free(buf);
}

#endif