#ifndef STRING_H

#include <malloc.h>

typedef struct
{
    char  *str;
    size_t size;
    size_t aloc_size;
} string_t;

string_t *string_create()
{
    string_t *str = malloc(sizeof(string_t));
    str->str = malloc(sizeof(char) * 512);
    str->size = 0;
    str->aloc_size = 512;
    return str;
}

void __string_realoc(string_t *str)
{
    char *new_str = malloc(sizeof(char) * (str->aloc_size + 512));
    for (size_t i = 0; i < str->size; i++) {
        new_str[i] = str->str[i]; 
    }
    free(str->str);
    str->str = new_str;
}

string_t *ztos(int z)
{
    string_t *str = malloc(sizeof(string_t));
    str->str = malloc(sizeof(char) * 512);
    str->size = 0;
    str->aloc_size = 512;

    size_t alcnum = (z / 512) + 1;

    for (size_t j = 0; j < alcnum; j++) {
        __string_realoc(str);
    }
    itoa(z, str->str, 10);

    return str;
}

void string_push_back(string_t *str, char val)
{
    if (str->size + 1 > str->aloc_size) {
        __string_realoc(str);
    }
    str->str[str->size++] = val;
}

void string_cat(string_t *str, char *val)
{
    for (size_t i = 0; i < strlen(val); i++) {
        string_push_back(str, val[i]);
    }
}

void string_free(string_t *str)
{
    free(str->str);
    free(str);
}

#endif