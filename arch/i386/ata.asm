section .text
global read_disk
extern puts
generic_disk:;read_disk(pntr_seg,pntr_off,C,H,S,D, n)
push bp
mov bp,sp
mov bx,[bp+4]
mov es,bx
mov bx,[bp+6]
mov ch,[bp+8]
mov dh,[bp+10]
mov cl,[bp+12]
mov dl,[bp+14]
mov al,[bp+16]
int 0x13
jc io_err_r
pop bp
ret

io_err_r:mov ax,-1
push err
call puts
add sp,2
pop bp
ret
section .data
err:db 'Read Error',0xa,0
global read_disk_lba
generic_disk_lba:; read_disk_lba(buf_seg,buf_off,drive,lba,count)
push bp
mov bp,sp
mov bx,[bp+4]
mov es,bx
mov bx,[bp+6]
mov dl,[bp+8]
push word [bp+10]
call getC
add sp,2
mov ch,al
push word [bp+10]
call getH
add sp,2
mov dh,al
push word [bp+10]
call getS
add sp,2
mov cl,al

mov al,[bp+12]
int 0x13
pop bp
ret
global getC
global getH
global getS
getS:
push bp
mov bp,sp
push dx
push bx
mov ax,[bp+4]
mov bx,63
div bx
mov ax,dx
inc ax
pop bx
pop dx
pop bp
ret
getH:
push bp
mov bp,sp
mov ah,[bp+4]
push dx
push bx
mov bx,63
div bx
mov bx,16
div bx
mov ax,dx
pop bx
pop dx
pop bp
ret
getC:push bp
mov bp,sp
push dx
push bx
mov ax,[bp+4]
mov bx,63*16
div bx
mov ax,dx
pop dx
pop bp
ret
read_disk:
push bp
mov bp,sp
mov ax,2
jmp generic_disk
global write_disk
write_disk:push bp
mov bp,sp
mov ah,3
jmp generic_disk
read_disk_lba:
push bp
mov bp,sp
mov ah,2
jmp generic_disk_lba
global write_disk_lba
write_disk_lba:
push bp
mov bp,sp
mov ah,3
jmp generic_disk_lba
