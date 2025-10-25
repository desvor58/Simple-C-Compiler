bits 16

global main

section .data

section .text
    main:
    push bp
    mov bp, sp
        mov word [bp - 2], 6
    pop bp
    ret

