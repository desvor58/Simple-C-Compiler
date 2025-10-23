bits 64

global main

section .data

section .text
    main:
    push rbp
    mov rbp, rsp
        call foo
        mov, rax, [rsp - 16]
        mov dword [rbp - 4], rax
    pop rbp
    ret

