#include "../test_template.h"
#include "../../src/types/vector.h"

genvector(int, 5)

int main(int argc, char **argv)
{
    test {
        log("vec init\n", 0);
        vector_int_t *vec = vector_int_create();
        if (vec->size) {
            err("Wrong init size\n",
                "    size:%u\n",
                "    aloc_size:%u\n",
                vec->size,
                vec->aloc_size);
        }

        size_t top = 50;
        for (size_t i = 0; i < top; i++) {
            log("vec add i%u\n", i);
            vector_int_push_back(vec, i);
            if (vec->size != i + 1) {
                err("Wrong vec size\n",
                    "    size:%u\n",
                    "    aloc_size:%u\n",
                    vec->size,
                    vec->aloc_size);
                break;
            }

            for (size_t j = 0; j < vec->size; j++) {
                log("vec get e%u\n", j);
                if (vec->arr[j] != j) {
                    err("Wrong element\n",
                        "    size:%u\n",
                        "    aloc_size:%u\n",
                        "    arr[j]:%d\n",
                        vec->size,
                        vec->aloc_size,
                        vec->arr[j]);
                    break;
                }
            }
        }
    } test_end;
}