#include "code/dev_test.h"

int puts(char *str)
{
    asm(
        "mov si, [bp - 4]"
        "mov ah, 01h"
        "int 21h"
    );
}

int main()
{
    puts("hello, world!");
}
