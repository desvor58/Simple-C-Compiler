#include "../test_template.h"
#include "../../src/types/string.h"

int main(int argc, char **argv)
{
    test {
        FILE *test_file = fopen("tests/common/string_test.txt", "r");
        if (!test_file) {
            err("Err:Test txt file not exist\n", 0);
            return 1;
        }

        char c = 0;
        /* --- string_push test --- */
            for (size_t t = 1; c != EOF; t++) {
                log("STRING_PUSH TEST %u\n", t);
                char *buf = malloc(sizeof(char) * 2048);
                for (size_t i = 0; i < 2048; i++) {
                    buf[i] = 0;
                }
                string_t *str = string_create("");
                if (str->size != 0) {
                    err("Err:Wrong init size\n"
                        "    size:%u\n",
                        str->size);
                }
                if (strlen(str->str) != 0) {
                    err("Err:Wrong init str\n"
                        "    str:%s\n",
                        str->str);
                }

                size_t i = 0;
                while ((c = fgetc(test_file)) != EOF) {
                    if (c == '\n') {
                        break;
                    }
                    log("push %c\n", c);
                    buf[i++] = c;
                    string_push_back(str, c);
                    log("cmp str and buf\n", 0);
                    if (strcmp(str->str, buf)) {
                        err("Err:str->str != buf\n"
                            "    str:%s\n"
                            "    buf:%s\n"
                            "    strcmp_ret:%d\n",
                            str->str,
                            buf,
                            strcmp(str->str, buf));
                    }
                }
                string_free(str);
                free(buf);
            }
        /* ------------------------ */

        /* --- string_cat test --- */
            // for (size_t t = 1; c != EOF; t++) {
            //     log("STRING_CAT TEST %u\n", t);
            //     char *buf = malloc(sizeof(char) * 512);
            //     for (size_t i = 0; i < 512; i++) {
            //         buf[i] = 0;
            //     }

            //     string_t *str = string_create("");
            //     if (str->size != 0) {
            //         err("Err:Wrong init size\n"
            //             "    size:%u\n",
            //             str->size);
            //     }
            //     if (strlen(str->str) != 0) {
            //         err("Err:Wrong init str\n"
            //             "    str:%s\n",
            //             str->str);
            //     }

            //     size_t i = 0;
            //     while (i < 512 && (c = fgetc(test_file)) != EOF) {
            //         if (c == '\n') {
            //             break;
            //         }
            //         log("push %c\n", c);
            //         buf[i++] = c;
            //     }
                
            // }
        /* ----------------------- */
    } test_end;
}