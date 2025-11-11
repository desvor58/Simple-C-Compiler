#ifndef STDIO_H
#define STDIO_H

int puts(char *str)
{
    asm(
        "mov si, [bp - 4]"
        "mov ah, 01h"
        "int 21h"
        "mov ah, 05h"
        "int 21h"
    );
}

#endif