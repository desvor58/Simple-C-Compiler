#include <stdio.h>
#include <string.h>

#include "../../src/types/list.h"
#include "../../src/types/hashmap.h"
#include "../../src/types/vector.h"
#include "../../src/types/string.h"

genlist(int)

FILE *logfile;
int err_num = 0;

#define gen_err(msg, ...)  \
    fprintf_s(logfile, msg, __VA_ARGS__);  \
    err_num++  \

int main(int argc, char **argv)
{
    fopen_s(&logfile, argv[1], "w");

    
    fprintf_s(logfile, "list init\n");
    list_int_pair_t *list = list_int_create();
    if (list_int_size(list)) {
        gen_err("Err:Wrong list init size (!= 0)\n"
                "    val:%u, next:%u\n",
                list->val,
                list->next);
    }

    int i0 = 5;
    fprintf_s(logfile, "list add i0\n");
    list_int_add(list, &i0);
    if (list_int_size(list) != 1) {
        gen_err("Err:Wrong list size (!= 1)\n"
                "    val:%u, next:%u\n",
                list->val,
                list->next);
    }
    
    fprintf_s(logfile, "list get i0\n");
    int *e0 = list_int_get(list, 0);
    if (*e0 != 5) {
        gen_err("Err:Wrong list element i0 (!= 5)\n"
                "    e:%u\n",
                *e0);
    }

    int i1 = 7;
    fprintf_s(logfile, "list add i1\n");
    list_int_add(list, &i1);
    if (list_int_size(list) != 2) {
        gen_err("Err:Wrong list size (!= 1)\n"
                "    val:%u, next:%u\n",
                list->val,
                list->next);
    }
    
    fprintf_s(logfile, "list get i1\n");
    int *e1 = list_int_get(list, 1);
    if (*e1 != 7) {
        gen_err("Err:Wrong list element e1 (!= 7)\n"
                "    e:%u\n",
                *e1);
    }
    
    fprintf_s(logfile, "list get i0\n");
    e0 = list_int_get(list, 0);
    if (*e0 != 5) {
        gen_err("Err:Wrong list element i0 (!= 5)\n"
                "    e:%u\n",
                *e0);
    }

        
    fclose(logfile);
    return err_num;
}