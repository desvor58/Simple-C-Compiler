bits 64

global main

section .data

section .text
    main:
    push rbp
    mov rbp, rsp
        mov dword [rbp - 4], rax
    pop rbp
    ret

