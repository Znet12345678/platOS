[bits 16]
section .text
global _putc
_putc:push bp
mov bp,sp
mov ah,0x0e
mov al,[bp+4]
int 0x10
pop bp
ret
