
[bits 16]
section .text
load_bin_id:dw 0xffff
load_aout_id:dw 0xfffe
load_elf_id:dw 0xfffd
load_pe_id:dw 0xfffc
load_macho_id:dw 0xfffb
addr:dw 0
exec_hdr:
a_midmag:dw 0
a_text: dw 0
a_data:dw 0
a_bss:dw 0
a_syms:dw 0
a_entry:dw 0
a_trsize:dw 0
a_drsize:dw 0
[bits 32]
global memcpy
global load_elf
memcpy:
push ebp
mov ebp,esp
mov edi,[ebp+8]
mov esi,[ebp+12]
mov ebx,[ebp+16]
mov ecx,0

.memcpyl:
mov al,[esi]
mov byte [edi],al
cmp ecx,ebx
je r32
inc ecx
inc edi
inc esi

jmp .memcpyl
r32:
pop ebp
ret
load_elf:push ebp
mov ebp,esp
mov esi,[ebp+8]
mov dword edi,[esi+0x1c]
mov ecx,0
mov  ebx,[esi+0x2c]
add edi,[ebp+8]
.el:
push dword [edi+0x10]
mov eax, [edi+0x4]
add eax,[ebp+8]
push eax
push dword [edi + 0x08]

call memcpy
add esp,12
cmp ecx,ebx
je e32
inc ecx
add edi,0x20
jmp .el
e32:mov edi,[ebp+8]
call [edi+0x18]
jmp panic
pop ebp
ret
load_bin:
push bp
mov bp,sp
mov ax,[bp+4]
mov ds,ax
mov es,ax
mov bx,addr
mov word [bx],ax
jmp 0x100:addr
pop bp
ret
load_pe:
push bp
mov bp,sp
pop bp
ret
load_macho:
push bp
mov bp,sp

pop bp
ret
panic:jmp panic
