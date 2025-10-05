global main
section .data
    global_var dd 17

section .text
main:
    push rbp
    mov rbp, rsp
    mov dword [rbp - 4], [rel global_var]
    mov qword [rbp - 12], dword [rbp - 4]
    pop rbp
    ret
