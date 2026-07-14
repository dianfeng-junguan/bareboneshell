;consts
VIDEO_ADDR equ 0xb8000
DISPLAY_PAGE_NUM equ 8

COLOR_GRAY equ 0x7
COLOR_RED equ 0xc
COLOR_BLUE equ 0x1
COLOR_GREEN equ 0xa
COLOR_LIGHTBLUE equ 0xb
org 0x7c00
jmp start
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

loop:
hlt
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
inc [cursor_y]
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

data_start:
title: db "bareshell is now running.",0
prompt: db "bareshell:>",0
cursor_x: dw 0
cursor_y: dw 0
times 510-($-$$) db 0
db 0x55,0xaa