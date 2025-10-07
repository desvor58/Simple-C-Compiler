global main
section .data
global_var dd 17
section .text
foo:
push rbp
mov rbp, rsp
mov dword [rbp - 4], 6
mov dword [rbp - 8], dword [rbp - 4]
pop rbp
ret
main:
push rbp
mov rbp, rsp
mov byte [rbp - 1], 0
mov dword [rbp - 5], -5
mov dword [rbp - 9], dword [rbp - 5]
mov rax, rbp - 1
mov rbx, 0
mov [rax], rbx
pop rbp
ret
