section .multiboot
global multiboot_header
multiboot_header:
dd 0x1BADB002
dd 0
dd -(0x1BADB002 )
framebuffer_tag_start:
dw 5
dw 1
dd 1
dd 80
dd 25
dd 32
fbend:
