[bits 16]
section .text
extern gdtdesc32
extern gdt
extern code
extern end
global _go_prot32
_go_prot32:
in al,0x92
or al,2
out 0x92,al
cli
lgdt [gdtdesc32]
mov eax,cr0
or al,1
mov cr0,eax

jmp 0x8:PModeMain
[bits 32]
PModeMain:
xor eax,eax
xor ebx,ebx
xor ecx,ecx
xor edx,edx
;mov esp,0xffff
mov eax,16
mov ds,eax
mov eax,24
mov ss,eax
mov eax,32
mov es,eax
mov eax,40
mov gs,eax
mov eax,48
mov fs,eax
;jmp 0x10200
mov esp,0xa00000
mov ebp,esp

jmp 0x8:0x10000
;mov word [0xb8000],'A'
h:jmp h

