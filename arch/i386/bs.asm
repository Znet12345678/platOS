[bits 16]
[org 0x7c24]
s:xor ax,ax

mov es,ax
mov ax,0xff0
mov ss,ax
mov ah,0x02

mov al,6
mov ch,0
mov cl,2
mov dh,0
mov bx,0xff0
int 0x13
jc err
mov byte [0xf000],dl
mov bx,[0xffa]
jmp bx
err:
jmp err


