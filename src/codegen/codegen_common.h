#ifndef CODEGEN_COMMON_H
#define CODEGEN_COMMON_H

#include "../types.h"
#include "../types/hashmap.h"

typedef struct {
    size_t locvar_offset;
} codegen_namespace_info_t;
genlist(codegen_namespace_info_t);

typedef struct {
    size_t  rbp_offset;
    ctype_t type;
    int     isstatic;
} codegen_var_info_t;
genhashmap(codegen_var_info_t)

#endif