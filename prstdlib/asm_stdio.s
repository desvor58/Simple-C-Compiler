[BITS 16]
[ORG 0x8000]

puts:
    push bp
    mov bp, sp
        mov si, [bp - 2]
        mov ah, 0x01
        int 0x21
    pop bp
    ret