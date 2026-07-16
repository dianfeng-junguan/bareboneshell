extern main
global kernelhead_start
bits 32
jmp kernelhead_start
kernelhead_start:
pm_start:
mov ax,0x10
mov ds,ax
mov es,ax
mov ax,0x18
mov ss,ax
jmp main
jmp $