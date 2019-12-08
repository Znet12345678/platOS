[bits 16]
memcpy:
push bp
mov bp,sp
mov di,[bp+4]
mov si,[bp+6]
mov ax,[bp+8]
.loop:
mov bx,[si]
mov [di],bx
cmp ax,0
inc si
inc di
je r
dec ax
jmp .loop
r:
pop bp
ret

