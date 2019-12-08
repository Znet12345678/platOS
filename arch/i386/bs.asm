[bits 16]
[org 0x7c00]
search:
mov [0x100],dl
mov ax,0
mov ds,ax
mov ah,0x42
mov si,dap
int 0x13
jc e
cmp byte [0x1002],0x7f
jne l
cmp byte [0x1003],'p'
jne l
cmp byte [0x1004],'l'
jne l
cmp byte [0x1005],'a'
jne l
cmp byte [0x1006],'t'
jne l
cmp byte [0x1007],'O'
jne l
cmp byte [0x1008],'S'
jne l
mov ah,0x0e
mov al,'F'
int 0x10
inc word [dap5]
mov ax,[0x1009]
mov word [0x104],ax
rr:
mov ax,[0x104]
mov word [dap2],ax
cmp word [dap2],63
jg fix
return:
mov ax,0
mov ds,ax
mov si,dap
mov ah,0x42
int 0x13
jc e

mov ax,[dap2]
add word [dap5],ax
sub word [0x104],ax
add dword [dap3],63*512
cmp word [0x104],0
jne h
jmp epmode
l:inc word [dap5]
jmp search
fix:
mov word [dap2],63
jg return
dap:
dap0:db 0x10
dap1:db 0
dap2:dw 1
dap3:dw 0x1000
dap4:dw 0
dap5:dq 0
e:mov al,ah
add al,'0'
mov ah,0x0e
int 0x10
h:jmp h
epmode:
cli 
lgdt [gdtdesc32]
mov eax,cr0
or al,1
mov cr0,eax
jmp 0x8:loadelf
gdt:
a:
null:dq 0
code:dw 0xffff
dw 0
db 0
db 0b10011010
db 0b11001111
db 0
data:dw 0xffff
dw 0
db 0
db 0b10010010
db 0b11001111
db 0
stack:dw 0xffff
dw 0
db 0
db 0b10010010
db 0b11001111
db 0
end:
global gdtdesc32
gdtdesc32: dw end-a-1
dd gdt
[bits 32]
tmp:dd 0
e_phnum:dd 0
base:dd 0
loadelf:
mov eax,24
mov ss,eax
mov eax,16
mov ds,eax
mov ax,[0x1000+44]
mov word [e_phnum],ax
mov ebx,0
phl:
mov esi,[0x1000+28]
add esi,0x1004
add esi,[base]
mov esi,[esi]
add esi,0x1000
mov edi,[0x1000+28]
add edi,0x1008
add edi,[base]
mov edi,[edi]
mov ecx,0
mov eax,[0x1000+28]
add eax,0x1014
add eax,[base]
push ebx
mov ebx,eax
mov ebx,[ebx]
mov eax,ebx
pop ebx

phil:
mov dl,[esi]
mov byte [edi],dl
inc edi
inc esi
cmp ecx,eax
je f2
inc ecx
jmp phil
f2:
add dword [base],0x20

cmp dword ebx,[e_phnum]
jge f
inc ebx
jmp phl
f:jmp dword [0x1000+24]
times 510 - ($ - $$) db 0
dw 0xaa55
