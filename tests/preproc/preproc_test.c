#include "../test_template.h"
#include "../../src/preproc.h"

int main(int argc, char **argv)
{
    test {

        list_error_t_pair_t *err_stk = list_error_t_create();

        log("list created\n", 0);

        for (size_t test_num = 0;;test_num++) {
            puts("a");
            log("TEST %u\n", test_num);

            FILE *test_code_file;
            FILE *test_xout_file;
            fopen_s(&test_code_file, format("tests/preproc/test%u.code", test_num), "r");
            if (!test_code_file) {
                log(".code file not exist (test%u.code) (test %u)\n", test_num, test_num);
                break;
            }
            fopen_s(&test_xout_file, format("tests/preproc/test%u.xout", test_num), "r");
            if (!test_xout_file) {
                err(".xout file not exist (test %u)\n", test_num);
                break;
            }

            string_t *code = string_create("");
            log("get code text\n", 0);
            get_file_text(test_code_file, code);

            string_t *xout = string_create("");
            log("get xout text\n", 0);
            get_file_text(test_xout_file, xout);

            fclose(test_code_file);
            fclose(test_xout_file);

            preproc_info_t *preproc = preproc_create(
                err_stk,
                (args_t){
                    .entry_fun_name = "main",
                    .target = TRGT_x8086,
                    .flags = 0,
                    .outfile_name = "a.exe"
                },
                code,
                format("test%u.code", test_num)
            );
            log("preproc text\n", 0);
            puts("ps");
            preprocess(preproc);
            puts("pe");

            log("cmp code and xout\n", 0);
            if (strcmp(code->str, xout->str)) {
                err("Err:code != xout\n"
                    "    code:%s\n"
                    "    xout:%s\n",
                    code->str,
                    xout->str);
            }
            puts("c");
            preproc_delete(preproc);
            string_free(code);
            string_free(xout);
        }
    } test_end;
}