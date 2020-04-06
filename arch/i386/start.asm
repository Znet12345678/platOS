[bits 16]
section .text
global _start
extern _puts
extern _err
extern _main
_start:
push bp
mov bp,sp
mov dl,[0xf000]
jmp _main
pop bp
ret
global _inter
_inter:
push bp
mov bp,sp
mov dl,[0xf000]
mov ax,dap
mov si,ax
mov ah,0x42
int 0x13
jc err
pop bp
ret
extern _putx
global __ata_read
__ata_read:
push bp
mov bp,sp
mov ax,[bp+4]
mov word [buf],ax
mov ax,[bp+6]
mov word [lba],ax
mov ax,[bp+8]
mov word [n],ax
mov ax,[bp+10]
mov word [eg],ax
call _inter
pop bp
ret
err:push bp
mov bp,sp
xor al,al
shr ax,8
push ax
push msg
call _puts
add sp,2
call _putx
add sp,2
hng:jmp hng
section .data
dap:
db 0x10
db 0
n:dw 1
buf:dw 0
eg:dw 0
lba:dq 0
msg:db "ATA Error 0x"
