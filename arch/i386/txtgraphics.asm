[bits 16]
global lines
global seek
global putc
global line
global rect
global puts
puts:
push bp
mov bp,sp
mov si,[bp+4]
l:
cmp byte [si],0
je r
mov ah,0x0e
mov al,[si]
;int 0x10
inc si
jmp l
pop bp
ret
putc:; Character Color N
push bp
mov bp,sp
mov al,[bp+4]
mov bx,[bp+6]
mov cx,[bp+8]
cmp al,0xa
je nl
mov ah,0x09
;int 0x10
mov ah,0x03
mov bh,0
;int 0x10
inc dl
cmp dl,80
jge nl
mov ah,2
;int 0x10
pop bp
ret
nl:mov bh,0
mov ah,3
;int 0x10
inc dh
mov dl,0
mov ah,2
;int 0x10
pop bp
ret
seek:push bp
mov bp,sp
mov ah,2
mov bh,0
mov dx,[bp+4]
;int 0x10
pop bp
ret
;Draw line ((col) << 8 | row),char,n,color
line:
push bp
mov bp,sp
mov ax,[bp+4]
push ax
call seek
add sp,2
mov ax,[bp+6]
mov bx,[bp+10]
mov cx,[bp+8]
push cx
push bx
push ax
call putc
add sp,6
pop bp
ret
;Draw rectange ((col) << 8 | row),char,l,w,color
rect:
push bp
mov bp,sp
mov di,[bp+8]
mov si,[bp+4]
.loop:
push word [bp+12]
push word [bp+10]
push word [bp+6]
push si
call line
add sp,8
cmp di,0
je r
dec di
add si,0x100

jmp .loop
r:pop bp
ret
lines:
;Put string with colors ((col << 8 | row),str,color
push bp
mov bp,sp
push word [bp+4]
call seek
add sp,2
mov si,[bp+6]
.loop2:
cmp byte [si],0
je r2
push word 1
push word [bp+8] 
push word [si]
call putc
add sp,6
inc si
jmp .loop2
r2:pop bp
ret
