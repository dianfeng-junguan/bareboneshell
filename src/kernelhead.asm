extern main
global kernelhead_start
bits 32
jmp kernelhead_start
kernelhead_start:
mov ax,0x10
mov ds,ax
mov es,ax
mov ax,0x18
mov ss,ax
mov esp,0x7c00

; set up idt

jmp main
jmp $

section .data
global idt
global idtr
idtr:
    dw 0x7ff ; =0x800-1
    dd idt
idt:
    times 256 dq 0