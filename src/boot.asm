;consts
VIDEO_ADDR equ 0xb8000
DISPLAY_PAGE_NUM equ 8

COLOR_GRAY equ 0x7
COLOR_RED equ 0xc
COLOR_BLUE equ 0x1
COLOR_GREEN equ 0xa
COLOR_LIGHTBLUE equ 0xb

extern main
[bits 16]
org 0x7c00
jmp start

data_start:
gdt:
    dq 0 ; dull descriptor
    ; kernel code descriptor
    dw 0xffff ; limit low
    dw 0 ; base low
    db 0 ; base mid
    db 0x9a ;access byte. present, DPL=0, code, non-conforming, readable
    db 0xcf ; flags byte & limit high. granuality 4k, 32bit
    db 0 ; base high
    ; kernel data descriptor
    dw 0xffff ; limit low
    dw 0 ; base low
    db 0 ; base mid
    db 0x92 ;access byte. present, DPL=0, data, increasing, writable
    db 0xcf ; flags byte & limit high. granuality 4k, 32bit
    db 0 ; base high
    ; kernel data(stack) descriptor
    dw 0xffff ; limit low
    dw 0 ; base low$(BIN_DIR)/boot.o 
    db 0 ; base mid
    db 0x92 ;access byte. present, DPL=0, data, increasing, writable
    db 0xcf ; flags byte & limit high. granuality 4k, 32bit
    db 0 ; base high

gdtr:
    dw 0x1f;0x20-1=8*4-1
    dd gdt
a20_ns: db "A20 is not supported on this machine. Cannot run the program.",0
a20_failmsg: db "Failed to enable the A20. There might be some problems with the program",0
disk_read_address_packet:
    db 0x10
    db 0
    dw 0x20 ; 32 sectors to read
    ; so we will transfer data to 0x7e00, right after the bootloader
    dw 0 ; offset addr as dst
    dw 0x7e0 ;segment address as dst
    dq 1 ; lba addr
title: db "bareshell is now running.",0
prompt: db "bareshell:>",0
cursor_x: dw 0
cursor_y: dw 0

start:
mov ax,cs
mov es,ax
mov ds,ax

mov ax,0x7c00
mov sp,ax
; get video mode
;INT 10 - VIDEO - GET CURRENT VIDEO MODE
;	AH = 0Fh
;Return: AH = number of character columns
;	AL = display mode (see #00010 at AH=00h)
;	BH = active page (see AH=05h)
;Notes:	if mode was set with bit 7 set ("no blanking"), the returned mode will
;	  also have bit 7 set
mov ax,0
mov ah,0xf
int 0x10
cmp al,0x3
je .set_video_mode_done

; set video modes
mov ax,0
mov ah,0x3
int 0x10
.set_video_mode_done:
; set gs as video buffer base
mov ax,0xb800
mov gs,ax

call cls

mov di,title
mov ax,ds
sub di,ax
mov ah,COLOR_GRAY
call print

call next_line

mov di,prompt
mov ax,ds
sub di,ax
mov ah,COLOR_GRAY
call print

; now load the program
mov ax,0
mov ds,ax ; we directly put address of data in si for convenience
mov si,disk_read_address_packet
mov ah,0x42
mov dl,0x80 ; set drive number
int 0x13

; see if A20 is supported
mov ax,0x2403
int 0x15
jc a20_unsupported
test ah,ah
jnz a20_unsupported

; test the a20 wire
mov     ax, 0x2402              ; Get A20 gate status
int     0x15
jc      a20_failed              ; Couldn't get status
test    ah, ah
jnz     a20_failed              ; Couldn't get status
test    al, al
jnz enter_protected_mode    ; already enabled

; now we enable the A20
; in al,0x92
; or al,2
; out 0x92,al
mov ax,0x2401
int 0x15
jc a20_failed
test ah,ah
jnz a20_failed

; enter the protected mode where we will be able to use c
enter_protected_mode:
cli            ; disable interrupts
lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
mov eax, cr0 
or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
mov cr0, eax
mov eax,0x7e00
; jump to program
jmp dword 0x8:0x7e00


loop:
hlt
jmp $
a20_failed:
mov si,a20_failmsg
mov ax,ds
sub si,ax
mov ah,COLOR_RED
call print
jmp $
a20_unsupported:
mov si,a20_ns
mov ax,ds
sub si,ax
mov ah,COLOR_RED
call print
jmp $
cls:
push si
push cx
mov cx,4000
mov si,0
.loop:
mov byte gs:[si],0
inc si
loop .loop

pop cx
pop si
ret
next_line:
inc word [cursor_y]
push ax
mov ax,0
mov word [cursor_x],ax
pop ax
ret
print:
; ds:di=str addr
; ah=color
push bx
; read from cursor
mov bx,[cursor_y]
push ax
mov ax,bx
shl bx,6 ;*64
shl ax,4 ;*16
add bx,ax; *80=*64+*16
pop ax
add bx,[cursor_x]
shl bx,1 ; 2 bytes per char
; si = (y*80+x)*2
mov si,bx ; store it to the pointer

.loop:
mov bh,ds:[di]
cmp bh,0
je .over
mov byte gs:[si],bh
inc di
inc si
mov byte gs:[si],ah
inc si


jmp .loop
.over:
pop bx
ret

times 510-($-$$) db 0
db 0x55,0xaa