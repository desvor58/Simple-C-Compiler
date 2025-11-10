#ifndef PREPROC_H
#define PREPROC_H

#include "types.h"
#include "types/hashmap.h"

typedef struct {
    list_error_t_pair_t *err_stk;
    args_t     args;
    string_t  *text;
    string_t  *buf;
    size_t     pos;
    char      *file;
    size_t     line;
    size_t     chpos;
} preproc_info_t;

preproc_info_t *preproc_create(list_error_t_pair_t *err_stk, args_t args, string_t *text, char *file)
{
    preproc_info_t *preproc = malloc(sizeof(preproc_info_t));
    preproc->err_stk = err_stk;
    preproc->args    = args;
    preproc->text    = text;
    preproc->buf     = string_create("");
    preproc->pos     = 0;
    preproc->file    = file;
    preproc->line    = 1;
    preproc->chpos   = 0;
    return preproc;
}

void preproc_delete(preproc_info_t *preproc)
{
    string_free(preproc->buf);
    free(preproc);
}

void preproc_skip_notoks(preproc_info_t *preproc);
void preproc_gettok(preproc_info_t *preproc);

void preproc_derective_include(preproc_info_t *preproc, size_t start_pos);
void preproc_derective_define(preproc_info_t *preproc, size_t start_pos);
void preproc_derective_undef(preproc_info_t *preproc, size_t start_pos);
void preproc_derective_ifndef(preproc_info_t *preproc, size_t start_pos);

genhashmap(macro_info_t)

hashmap_macro_info_t_t *macros = 0;

void preprocess(preproc_info_t *preproc)
{
    // printf_s("start\n");
    int macros_be_created = 0;

    if (macros == 0) {
        macros = hashmap_macro_info_t_create();
        macros_be_created = 1;
    }

    string_insert(preproc->text, 0, "#file %s\n", preproc->file);
    while (preproc->text->str[preproc->pos] != '\n' && preproc->text->str[preproc->pos] != '\0')  {
        preproc->pos++;
    }
    preproc->pos++;

    for (preproc->pos = 0; preproc->pos < preproc->text->size; preproc->pos++) {
        if (preproc->text->str[preproc->pos] == '\n') {
            preproc->line++;
            preproc->chpos = 1;
        } else {
            preproc->chpos++;
        }
        if (preproc->text->str[preproc->pos] == '/' && preproc->text->str[preproc->pos + 1] == '/') {
            size_t start_pos = preproc->pos;
            while (preproc->text->str[preproc->pos] != '\n' && preproc->text->str[preproc->pos] != '\0') {
                preproc->pos++;
            }
            string_replace(preproc->text, start_pos, preproc->pos, "");
            preproc->pos = start_pos;
        } else
        if (preproc->text->str[preproc->pos] == '#') {
            u32 start_pos = preproc->pos;
            preproc_gettok(preproc);

            if (!strcmp(preproc->buf->str, "#file")) {} else
            if (!strcmp(preproc->buf->str, "#endif")) {
                string_replace(preproc->text, start_pos, preproc->pos, "");
            } else
            if (!strcmp(preproc->buf->str, "#ifndef")) {
                preproc_derective_ifndef(preproc, start_pos);
            } else
            if (!strcmp(preproc->buf->str, "#include")) {
                preproc_derective_include(preproc, start_pos);
            } else
            if (!strcmp(preproc->buf->str, "#define")) {
                preproc_derective_define(preproc, start_pos);
            } else
            if (!strcmp(preproc->buf->str, "#undef")) {
                preproc_derective_undef(preproc, start_pos);
            } else {
                generr(preproc->err_stk,
                       "Unknow preprocessor derective",
                       preproc->file,
                       preproc->line,
                       preproc->chpos);
                       printf_s("%s\n", preproc->buf->str);
            }
            preproc->pos--;
        } else
        if (isalpha(preproc->text->str[preproc->pos]) || preproc->text->str[preproc->pos] == '_') {
            size_t start_pos = preproc->pos;
            preproc_gettok(preproc);

            macro_info_t *macro = hashmap_macro_info_t_get(macros, preproc->buf->str);
            if (macro) {
                string_replace(preproc->text, start_pos, preproc->pos, macro->val);
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
    while (preproc->text->str[preproc->pos] == ' ') {
        preproc->pos++;
    }
    if (preproc->text->str[preproc->pos] == '"') {
        preproc->buf->size = 0;
        while (preproc->text->str[++preproc->pos] != '"') {
            string_push_back(preproc->buf, preproc->text->str[preproc->pos]);
        }
        string_push_back(preproc->buf, '\0');
        string_t *full_path = string_create("");

        int top = strlen(preproc->file) - 1;
        while (preproc->file[top] != '/' && preproc->file[top] != '\\' && top >= 0) {
            top--;
        }
        string_push_back(full_path, '.');
        string_push_back(full_path, '/');
        size_t i = 0;
        while (i <= top) {
            string_push_back(full_path, preproc->file[i]);
            i++;
        }

        string_cat(full_path, "%s", preproc->buf->str);
        string_push_back(full_path, '\0');
        //printf_s("full_path:%s, buf:%s\n", full_path->str, preproc->buf->str);
        
        FILE *included_file;
        fopen_s(&included_file, full_path->str, "r");
        if (!included_file) {
            generr(preproc->err_stk,
                   "No such included file",
                   preproc->file,
                   preproc->line,
                   preproc->chpos);
            return;
        }
        string_t *included_code = string_create("");
        get_file_text(included_file, included_code);
        fclose(included_file);

        preproc_info_t *_preproc = preproc_create(preproc->err_stk,
                                                  preproc->args,
                                                  included_code,
                                                  full_path->str);
        
        preprocess(_preproc);
        preproc_delete(_preproc);
        // printf_s("%s\n\tEND\n", included_code);
        // printf_s("%s\n", preproc->text);
        string_replace(preproc->text, start_pos, preproc->pos + 1, "%s", included_code->str);

        preproc->pos = start_pos + included_code->size;

        string_insert(preproc->text, preproc->pos, "#file %s\n", preproc->file);
        while (preproc->text->str[preproc->pos] != '\n' && preproc->text->str[preproc->pos] != '\0')  {
            preproc->pos++;
        }

        string_free(included_code);
        string_free(full_path);
    }
}

void preproc_derective_define(preproc_info_t *preproc, size_t start_pos)
{
    preproc_skip_notoks(preproc);
    preproc_gettok(preproc);
    macro_info_t *macro = (macro_info_t*)malloc(sizeof(macro_info_t));
    strcpy(macro->name, preproc->buf->str);
    while (preproc->text->str[preproc->pos++] == ' ') {}

    preproc->pos--;
    preproc->buf->size = 0;
    size_t i = 0;
    while (preproc->text->str[preproc->pos] != '\n' && preproc->text->str[preproc->pos] != '\0') {
        string_push_back(preproc->buf, preproc->text->str[preproc->pos++]);
    }
    string_push_back(preproc->buf, '\0');
    strcpy(macro->val, preproc->buf->str);

    hashmap_macro_info_t_set(macros, macro->name, macro);

    string_replace(preproc->text, start_pos, preproc->pos + 1, "\n");
    preproc->pos = start_pos;
}

void preproc_derective_undef(preproc_info_t *preproc, size_t start_pos)
{
    preproc_skip_notoks(preproc);
    preproc_gettok(preproc);
    int err = hashmap_macro_info_t_delete(macros, preproc->buf->str);
    if (err) {
        generr(preproc->err_stk,
               "unknow identifire",
               preproc->file,
               preproc->line,
               preproc->chpos);
    }
    string_replace(preproc->text, start_pos, preproc->pos + 1, "\n");
    preproc->pos = start_pos;
}

void preproc_skip_notoks(preproc_info_t *preproc)
{
    while (!isalpha(preproc->text->str[preproc->pos])
        && preproc->text->str[preproc->pos] != '_'
        && preproc->text->str[preproc->pos] != '#') {
        preproc->pos++;
    }
}

void preproc_gettok(preproc_info_t *preproc)
{
    int i = 0;
    preproc->buf->size = 0;
    while (isalpha(preproc->text->str[preproc->pos])
        || isdigit(preproc->text->str[preproc->pos])
        || preproc->text->str[preproc->pos] == '_'
        || preproc->text->str[preproc->pos] == '#'
    ) {
        string_push_back(preproc->buf, preproc->text->str[preproc->pos]);
        i++;
        preproc->pos++;
    }
    string_push_back(preproc->buf, '\0');
}

void preproc_derective_ifndef(preproc_info_t *preproc, size_t start_pos)
{
    preproc_skip_notoks(preproc);
    preproc_gettok(preproc);

    macro_info_t *macro = hashmap_macro_info_t_get(macros, preproc->buf->str);
    if (!macro) {
        string_replace(preproc->text, start_pos, preproc->pos, "");
        preproc->pos = start_pos;
        return;
    }
    for (;;) {
        if (preproc->text->str[preproc->pos++] != '#') {
            preproc_gettok(preproc);
            if (!strcmp(preproc->buf->str, "#endif")) {
                while (preproc->text->str[preproc->pos++] != '\n') {}
                string_replace(preproc->text, start_pos, preproc->pos, "");
                preproc->pos = start_pos;
                return;
            }
        }
    }
}

#endif