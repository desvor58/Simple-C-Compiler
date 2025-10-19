bits 64

global main

section .data

section .text
    main:
    push rbp
    mov rbp, rsp
        mov rax, 3
        mov rbx, 5
        add rax, rbx
        mov rbx, 2
        sub rax, rbx
        mov rbx, 5
        imul rax, rbx
        mov dword [rbp - 4], rax
    pop rbp
    ret

