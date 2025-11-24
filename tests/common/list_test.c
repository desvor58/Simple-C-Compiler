#include "../test_template.h"
#include "../../src/types/list.h"

genlist(int)

int main(int argc, char **argv)
{
    test {
        log("list init\n", 0);
        list_int_pair_t *list = list_int_create();
        if (list_int_size(list)) {
            err("Err:Wrong list init size (!= 0)\n"
                    "    val:%u, next:%u\n",
                    list->val,
                    list->next);
        }

        size_t top = 50;
        for (size_t i = 0; i < top; i++) {
            int *it = malloc(sizeof(int));
            *it = i;
            log("list add i%u\n", i);
            list_int_add(list, it);
            if (list_int_size(list) != i + 1) {
                err("Err:Wrong list size\n"
                    "    size:%u, next:%u\n",
                    list_int_size(list),
                    list->next);
            }

            for (size_t j = 0; j < list_int_size(list); j++) {
                log("list get e%u\n", j);
                int *el = list_int_get(list, j);
                if (*el != j) {
                    err("Err:Wrong list element\n"
                        "    e:%u\n",
                        *el);
                }
            }
        }
        list_int_full_free(list);
    } test_end;
}