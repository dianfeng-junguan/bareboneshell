bits 32
global init_keyboard
global ps2_send_command
global ps2_read_configure_byte
global ps2_write_configure_byte
global ps2_self_test
PS2_DATA equ 0x60
PS2_STATUS equ 0x64
PS2_COMMAND equ 0x64
ps2_send_command:
; edi=command
push dx
mov dx,PS2_COMMAND
mov ax,di
out dx,al
pop dx
ret
ps2_read_configure_byte:
push dx
; eax=config byte
; read config byte
mov al,0x20
mov dx,PS2_COMMAND
out dx,al

.wait:
mov dx,PS2_STATUS
in al,dx
and dx,2
cmp al,0
jmp .wait

; start reading
mov dx,PS2_DATA
in al,dx

pop dx
ret

ps2_write_configure_byte:
;edi = byte
push dx
mov ax,di
xchg ah,al
; write back
mov dx,PS2_COMMAND
mov al,0x60
out dx,al

.wait2:
mov dx,PS2_STATUS
mov al,dx
and al,2
cmp al,0
jne .wait2

mov dx,PS2_DATA
out dx,ah

pop dx
ret
ps2_self_test:
push dx
; perform self test
mov dx,PS2_COMMAND
mov al,0xaa
out dx,al
; wait for response
.wait3:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait3
mov dx,PS2_COMMAND
in al,dx
cmp al,0x55
jne .kb_self_test_failure

mov eax,0
pop dx
ret
.kb_self_test_failure:
mov eax,1
pop dx
ret

init_keyboard:
; disable ps2 device
mov dx,PS2_COMMAND
mov al,0xad
out dx,al

mov al,0xa7
out dx,al

; flush the output buffer
.clear_output:
mov dx,PS2_STATUS
in al,dx
and al,0x1
cmp al,0
je .clear_done

mov dx,PS2_DATA
in al,dx
jmp .clear_output
.clear_done:

; set config byte
mov al,0x20
mov dx,PS2_COMMAND
out dx,al

.wait:
mov dx,PS2_STATUS
in al,dx
and dx,2
cmp al,0
jmp .wait

; start reading
mov dx,PS2_DATA
in al,dx
; set bits
and al,0xae
mov ah,al

; write back
mov dx,PS2_COMMAND
mov al,0x60
out dx,al

.wait2:
mov dx,PS2_STATUS
mov al,dx
and al,2
cmp al,0
jne .wait2

mov dx,PS2_DATA
out dx,ah

; perform self test
mov dx,PS2_COMMAND
mov al,0xaa
out dx,al
; wait for response
.wait3:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait3
mov dx,PS2_COMMAND
in al,dx
cmp al,0x55
jne kb_self_test_failure

; check if there are 2 channels
; enable the 2nd port
mov dx,PS2_COMMAND
mov al,0xa8
out dx,al

; read config byte
mov al,0x20
mov dx,PS2_COMMAND
out dx,al

.wait4:
mov dx,PS2_STATUS
in al,dx
and dx,2
cmp al,0
jmp .wait4

; start reading
mov dx,PS2_DATA
in al,dx

; see if bit 5 set 
and al,0x20
cmp al,0
jne .two_channel_done

; 2 channeles, disable

.two_channel_done:
ret