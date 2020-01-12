[bits 32]
section .start
global _start
extern code_begin
extern code_end
extern libmem_init
extern main
_start:
mov esp,0x80000
push ebp
mov ebp,esp
call libmem_init
call main+0xC0000000
pop ebp
ret
