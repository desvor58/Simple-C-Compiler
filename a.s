global main
section .data
global_var dd 17
section .text
foo:
push rbp
mov rbp, rsp
mov dword [rbp - 4], 5
pop rbp
ret
main:
push rbp
mov rbp, rsp
mov dword [rbp - 8], -5
pop rbp
ret
