[bits 16]
section .text
extern gdtdesc32
extern load_elf
global _go_prot32
_go_prot32:
cli
lgdt [gdtdesc32]
mov eax,cr0
or al,1
mov cr0,eax
jmp 08h:PModeMain
[bits 32]
PModeMain:
mov eax,24
mov ss,eax
mov eax,16
mov ds,eax
xor eax,eax
xor ebx,ebx
xor ecx,ecx
xor edx,edx
xor esi,esi
xor edi,edi
xor ebp,ebp
xor esp,esp
mov esp,0x400000
mov ebp,esp

push ebp
mov ebp,esp
push 0x2500 
call load_elf
add esp,4
jmp PModeMain
section .kern
times (512 - ($ - $$)) db 0
