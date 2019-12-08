[bits 16]
section .data
section .text
extern puts
extern rect
extern lines
extern seek
extern _go_prot32
global _start
_start:
push bp
mov bp,sp
mov dword [0x21*4],0x2200
jmp s
times 512 db 0
s:mov ax,0
mov bx,' '
mov cx,80
mov dx,0b00010000
push dx
push cx
mov cx,25
push cx
push bx
push ax
call rect
add sp,10
mov ax,0
mov dx,0b00010000
mov ax,0
push ax
call seek
add sp,2
push str
call puts
add sp,2
jmp _go_prot32
hng:jmp hng
pop bp
ret
section .data
str:db "platOS Version 0 alpha",0
