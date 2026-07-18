bits 32
global init_keyboard
global ps2_send_command
global ps2_read_configure_byte
global ps2_write_configure_byte
global ps2_self_test
global ps2_check_2_channels_and_disable
global ps2_interface_test
global ps2_enable_first
global ps2_enable_second
global ps2_disable
global ps2_flush_output
global ps2_reset_device
global ps2_enable_scanning
PS2_DATA equ 0x60
PS2_STATUS equ 0x64
PS2_COMMAND equ 0x64
ps2_send_command:
; edi=command
push edx
mov dx,PS2_COMMAND
mov ax,di
out dx,al
pop edx
ret
ps2_read_configure_byte:
push edx
; eax=config byte
; read config byte
mov al,0x20
mov dx,PS2_COMMAND
out dx,al

.wait:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait

; start reading
mov dx,PS2_DATA
mov ax,0
in al,dx

pop edx
ret

ps2_write_configure_byte:
push edi
;edi = byte
push edx
mov edi,[esp+12]
; write back
mov dx,PS2_COMMAND
mov al,0x60
out dx,al

.wait2:
mov dx,PS2_STATUS
in al,dx
and al,2
cmp al,0
jne .wait2

mov ax,di
mov dx,PS2_DATA
out dx,al

pop edx
pop edi
ret
ps2_self_test:
push edx
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
; read test result
mov dx,PS2_DATA
in al,dx
cmp al,0x55
jne .kb_self_test_failure

mov eax,0
pop edx
ret
.kb_self_test_failure:
mov eax,1
pop edx
ret

ps2_disable:
push edx
; disable ps2 device
mov dx,PS2_COMMAND
mov al,0xad
out dx,al

mov al,0xa7
out dx,al
pop edx
ret

; flush the output buffer
ps2_flush_output:
push edx
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
pop edx
ret

ps2_check_2_channels_and_disable:
push edx
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
and al,1
cmp al,0
je .wait4

; start reading
mov dx,PS2_DATA
in al,dx

; see if bit 5 set 
and al,0x20
cmp al,0
jne .two_channel_done

; 2 channeles, disable
mov ax,0xa7
mov dx,PS2_COMMAND
out dx,al

; config byte to disable irqs and clock for port 2
mov al,0x20
mov dx,PS2_COMMAND
out dx,al

.wait:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait

; start reading
mov dx,PS2_DATA
in al,dx
; set bits
and al,0xdd
mov ah,al

; write back
mov dx,PS2_COMMAND
mov al,0x60
out dx,al

.wait2:
mov dx,PS2_STATUS
in al,dx
and al,2
cmp al,0
jne .wait2

mov dx,PS2_DATA
mov al,ah
out dx,al

mov byte [ps2_has_two_channels], 1
mov ax,1 
pop edx
ret
.two_channel_done:
mov byte [ps2_has_two_channels], 0
mov ax,0
pop edx
ret
ps2_interface_test:
push edx
; test the ports
mov dx,PS2_COMMAND
mov ax,0xab
out dx,al
; wait for result
.wait:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait

; read and check result
mov dx,PS2_DATA
in al,dx
cmp al,0
; store which port failed in ax
mov ax,1
jne .kb_interface_test_failure

; if there are 2 channels, test the 2nd port
cmp byte [ps2_has_two_channels], 0
je .check_port_2_done

mov dx,PS2_COMMAND
mov ax,0xab
out dx,al

; wait for result
.wait2:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait2

; read and check result
mov dx,PS2_DATA
in al,dx
cmp al,0
; store which port failed in ax
mov ax,2
jne .kb_interface_test_failure
.check_port_2_done:
mov ax,0
pop edx
ret
.kb_interface_test_failure:

pop edx
ret
ps2_enable_first:
push edx
mov dx,PS2_COMMAND
mov ax,0xae
out dx,al

pop edx
ret
ps2_enable_second:
push edx
mov dx,PS2_COMMAND
mov ax,0xa8
out dx,al

pop edx
ret
ps2_reset_device:
push edx
push ebx
mov bx,0
mov dx,PS2_DATA
mov ax,0xff
out dx,al
; wait for response
.wait:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait

.read_loop:
mov dx,PS2_DATA
in al,dx
cmp al,0xfc
je .ps2_reset_failed
; comapre the response with 0xfa 0xaa followed by dev id
cmp al,0xfa
jne .second_char
cmp bx,0
jne .ps2_reset_failed
inc bx
.second_char:
.wait2:
mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
je .wait2

cmp al,0xaa
je .ps2_reset_success
cmp bx,1
jne .ps2_reset_failed

.ps2_reset_success:
; go on reading
mov dx,PS2_DATA
in al,dx

mov dx,PS2_STATUS
in al,dx
and al,1
cmp al,0
jne .ps2_reset_success ; dev id not finished yet

mov ax,0
pop ebx
pop edx
ret
.ps2_reset_failed:
mov ax,1
pop ebx
pop edx
ret
ps2_enable_scanning:
push edx
; wait for the output buffer to be empty
.wait:
mov dx,PS2_STATUS
in al,dx
and al,2
cmp al,0
jne .wait

; send the command
mov dx,PS2_DATA
mov al,0xf4
out dx,al

pop edx
ret
section .data
ps2_has_two_channels db 0