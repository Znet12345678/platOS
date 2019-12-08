[bits 16]
section .isr
push bp
mov bp,sp

r:pop bp
iret
