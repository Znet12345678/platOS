[bits 32]
section .data
msg:db 'Page fault for address 0x',0
msg2:db ' ->',0xa,0
msg3:db 0xa,'hng()',0
address:dd 0
inst:dd 0
section .text
global page
extern dbg
extern puts
extern putx
page:
mov eax,[esp+4]
mov dword [inst],eax
mov eax,cr2
mov dword [address],eax
push msg
call puts
add esp,4
push dword [address]
call putx
add esp,4
push msg2
call puts
add esp,4
push dword [inst]
call putx
add esp,4
jmp dbg
push msg3
call puts
add esp,4
hng:hlt
jmp hng
