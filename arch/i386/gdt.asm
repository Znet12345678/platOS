[bits 16]
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

