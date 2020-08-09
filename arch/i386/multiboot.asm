section .multiboot
global multiboot_header
extern _start
multiboot_header:
dd 0x1BADB002
dd 4
dd -(4+0x1BADB002)
dd 0
dd 0
dd 0
dd 0
dd 0
dd 0
dd 320
dd 200
dd 8
multiboot_end:
