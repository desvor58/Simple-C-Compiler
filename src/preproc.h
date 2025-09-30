#ifndef PREPROC_H
#define PREPROC_H

#include "types.h"
#include "types/hashmap.h"

typedef struct {
    args_t args;
    char  *text;
    char  *buf;
    size_t pos;
    char  *file;
    size_t line;
    size_t chpos;
} preproc_info_t;

preproc_info_t *preproc_create(args_t args, char *text, char *file)
{
    preproc_info_t *preproc = malloc(sizeof(preproc_info_t));
    preproc->args  = args;
    preproc->text  = text;
    preproc->buf   = malloc(sizeof(char)*MAX_MACRO_VAL);
    preproc->pos   = 0;
    preproc->file  = file;
    preproc->line  = 1;
    preproc->chpos = 0;
    return preproc;
}

void preproc_delete(preproc_info_t *preproc)
{
    free(preproc->buf);
    free(preproc);
}

void preproc_skip_notoks(preproc_info_t *preproc);
void preproc_gettok(preproc_info_t *preproc);

void preproc_derective_include(preproc_info_t *preproc, size_t start_pos);
void prerpoc_derective_define(preproc_info_t *preproc, size_t start_pos);
void prerpoc_derective_undef(preproc_info_t *preproc, size_t start_pos);

genhashmap(macro_info_t)

hashmap_macro_info_t_t *macros = 0;

static vector_error_t_t *err_stk;

void preprocess(preproc_info_t *preproc)
{
    // printf_s("start\n");
    int macros_be_created = 0;

    if (macros == 0) {
        macros = hashmap_macro_info_t_create();
        macros_be_created = 1;
    }

    for (preproc->pos = 0; preproc->pos < strlen(preproc->text); preproc->pos++) {
        if (preproc->text[preproc->pos] == '\n') {
            preproc->line++;
            preproc->chpos = 1;
        } else {
            preproc->chpos++;
        }
        if (preproc->text[preproc->pos] == '/' && preproc->text[preproc->pos + 1] == '/') {
            size_t start_pos = preproc->pos;
            while (preproc->text[preproc->pos] != '\n' && preproc->text[preproc->pos] != '\0') {
                preproc->pos++;
            }
            buf_replace(preproc->text, MAX_CODE_SIZE, start_pos, preproc->pos, "");
            preproc->pos = start_pos;
        } else
        if (preproc->text[preproc->pos] == '#') {
            u32 start_pos = preproc->pos;
            preproc_gettok(preproc);

            if (!strcmp(preproc->buf, "#include")) {
                preproc_derective_include(preproc, start_pos);
            } else
            if (!strcmp(preproc->buf, "#define")) {
                prerpoc_derective_define(preproc, start_pos);
            } else
            if (!strcmp(preproc->buf, "#undef")) {
                prerpoc_derective_undef(preproc, start_pos);
            } else {
                vector_error_t_push_back(err_stk, gen_error("Unknow preprocessor derective",
                                                            preproc->file,
                                                            preproc->line,
                                                            preproc->chpos));
            }
            preproc->pos--;
        } else
        if (isalpha(preproc->text[preproc->pos]) || preproc->text[preproc->pos] == '_') {
            size_t start_pos = preproc->pos;
            preproc_gettok(preproc);
            
            macro_info_t *macro = hashmap_macro_info_t_get(macros, preproc->buf);
            if (macro) {
                int err = buf_replace(preproc->text, MAX_CODE_SIZE, start_pos, preproc->pos, macro->val);
                if (err) {
                    put_error(gen_error("too large translation unit",
                                        preproc->file,
                                        preproc->line,
                                        preproc->chpos), 1);
                }
                preproc->pos = start_pos;
            }
        }
    }

    if (macros_be_created) {
        hashmap_macro_info_t_free(macros);
    }
    // puts("end");
}

void preproc_derective_include(preproc_info_t *preproc, size_t start_pos)
{
    while (preproc->text[preproc->pos] == ' ') {
        preproc->pos++;
    }
    if (preproc->text[preproc->pos] == '"') {
        int i;
        preproc->buf[0] = '\0';
        for (i = 0; preproc->text[++preproc->pos] != '"'; i++) {
            preproc->buf[i] = preproc->text[preproc->pos];
        }
        preproc->buf[i] = '\0';
        char *full_path = malloc(128);
        get_folder_path(preproc->file, full_path);
        strcat_s(full_path, 128, preproc->buf);
        FILE *included_file;
        fopen_s(&included_file, full_path, "r");
        free(full_path);
        if (!included_file) {
            vector_error_t_push_back(err_stk, gen_error("No such included file",
                                                        preproc->file, preproc->line,
                                                        preproc->chpos));
            return;
        }
        string_t *included_code = string_create();
        get_file_text(included_file, included_code);
        fclose(included_file);

        preproc_info_t *_preproc = preproc_create(preproc->args, included_code->str, preproc->file);
        
        preprocess(_preproc);
        preproc_delete(_preproc);
        // printf_s("%s\n\tEND\n", included_code);
        // printf_s("%s\n", preproc->text);

        int err = buf_replace(preproc->text, MAX_CODE_SIZE, start_pos, preproc->pos + 1, included_code->str);
        if (err) {
            put_error(gen_error("too large included file",
                                preproc->file,
                                preproc->line,
                                preproc->chpos), 1);
        }

        preproc->pos = start_pos + strlen(included_code->str);

        string_free(included_code);
    }
}

void prerpoc_derective_define(preproc_info_t *preproc, size_t start_pos)
{
    preproc_skip_notoks(preproc);
    preproc_gettok(preproc);
    macro_info_t *macro = (macro_info_t*)malloc(sizeof(macro_info_t));
    strcpy(macro->name, preproc->buf);
    while (preproc->text[preproc->pos++] == ' ') {}

    preproc->pos--;
    size_t i = 0;
    while (preproc->text[preproc->pos] != '\n' && preproc->text[preproc->pos] != '\0') {
        preproc->buf[i++] = preproc->text[preproc->pos++];
    }
    preproc->buf[i] = '\0';
    strcpy(macro->val, preproc->buf);

    hashmap_macro_info_t_set(macros, macro->name, macro);

    int err = buf_replace(preproc->text, MAX_CODE_SIZE, start_pos, preproc->pos + 1, "\n");
    if (err) {
        put_error(gen_error("too large translation unit",
                            preproc->file,
                            preproc->line,
                            preproc->chpos), 1);
    }
    preproc->pos = start_pos;
}

void prerpoc_derective_undef(preproc_info_t *preproc, size_t start_pos)
{
    preproc_skip_notoks(preproc);
    preproc_gettok(preproc);
    int err = hashmap_macro_info_t_delete(macros, preproc->buf);
    if (err) {
        vector_error_t_push_back(err_stk, gen_error("unknow identifire",
                                                    preproc->file,
                                                    preproc->line,
                                                    preproc->chpos));
    }
    err = buf_replace(preproc->text, MAX_CODE_SIZE, start_pos, preproc->pos + 1, "\n");
    if (err) {
        put_error(gen_error("too large translation unit",
                            preproc->file,
                            preproc->line,
                            preproc->chpos), 1);
    }
    preproc->pos = start_pos;
}

void preproc_skip_notoks(preproc_info_t *preproc)
{
    while (!isalpha(preproc->text[preproc->pos]) && preproc->text[preproc->pos] != '_' && preproc->text[preproc->pos] != '#') {
        preproc->pos++;
    }
}

void preproc_gettok(preproc_info_t *preproc)
{
    int i = 0;
    while (isalpha(preproc->text[preproc->pos])
        || isdigit(preproc->text[preproc->pos])
        || preproc->text[preproc->pos] == '_'
        || preproc->text[preproc->pos] == '#'
    ) {
        preproc->buf[i] = preproc->text[preproc->pos];
        i++;
        preproc->pos++;
    }
    preproc->buf[i] = '\0';
}

#endif