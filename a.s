bits 64
global main
section .data
global_var dd 17
section .text
foo:
push rbp
mov rbp, rsp
mov dword [rbp - 4], r8
mov byte [rbp - 5], r9
mov dword [rbp - 9], 6
mov rax, dword [rbp - 9]
mov dword [rbp - 13], rax
pop rbp
ret
main:
push rbp
mov rbp, rsp
mov dword [rbp - 4], -5
mov rax, dword [rbp - 4]
mov dword [rbp - 8], rax
pop rbp
ret
