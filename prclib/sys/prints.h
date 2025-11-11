/********************
 * PRos interruptions
 ********************/

#ifndef PRINTS_H
#define PRINTS_H

#define TC_BLACK        0
#define TC_DARK_BLUE    1
#define TC_DARK_GREEN   2
#define TC_DARK_CYAN    3
#define TC_DARK_RED     4
#define TC_DARK_MAGENTA 5
#define TC_BROWN        6
#define TC_LIGHT_GRAY   7
#define TC_DARK_GRAY    8
#define TC_BLUE         9
#define TC_GREEN        10
#define TC_CYAN         11
#define TC_RED          12
#define TC_MAGENTA      13
#define TC_YELLOW       14
#define TC_WHITE        15

int init_out_sys()
{
    asm(
        "mov ah, 00h"
        "int 21h"
    );
}

int set_text_col(char col)
{
    asm(
        "mov bl, [bp - 2]"
        "mov ah, 07h"
        "int 21h"
    );
}

int print_str(char *str)
{
    asm(
        "mov si, [bp - 2]"
        "mov ah, 08h"
        "int 21h"
    );
}

int read_str(char *buf, int buf_size)
{
    
}

#endif