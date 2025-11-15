#ifndef TEST_TEMPLATE_H
#define TEST_TEMPLATE_H

#include <stdio.h>
#include <string.h>

FILE *logfile;
int err_num = 0;

#define log(msg, ...)  \
    fprintf_s(logfile, msg, __VA_ARGS__)

#define err(msg, ...)  \
    fprintf_s(logfile, msg, __VA_ARGS__);  \
    err_num++

#define test  \
    fopen_s(&logfile, argv[1], "w");

#define test_end  \
    fclose(logfile);  \
    return err_num

#endif