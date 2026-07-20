#include "ps2.h"
#include "print.h"
BOOL ps2_has_two_channels=false;
int ps2_init(){
    ps2_disable();
    ps2_flush_output();
    unsigned char config=ps2_read_configure_byte();
    config&=0xae;
    ps2_write_configure_byte(config);
    if(ps2_self_test()!=0){
        print("ps2 self test failed\n", COLOR_RED);
        return -1;
    }
    ps2_has_two_channels=ps2_check_2_channels_and_disable();
    if(ps2_interface_test()!=0){
        print("ps2 interface test failed\n", COLOR_RED);
        return -1;
    }
    ps2_enable_first();
    if(ps2_reset_device()!=0){
        print("ps2 reset device failed\n", COLOR_RED);
        return -1;
    }
    unsigned char config2=ps2_read_configure_byte();
    config2&=0xaf;
    config2|=0x1;// enable port 1 interrupt
    ps2_write_configure_byte(config2);
    ps2_enable_scanning();
    return 0;
}

#include "idt.h"
#include "ins.h"
// keyboard=================================
struct ps2_key_code{
    UINT8 scan_code;
    char ascii;
    char shift_ascii;
};
#define KEYBOARD_BUFFER_SIZE 64
static UINT8 keyboard_buffer[KEYBOARD_BUFFER_SIZE]={0};
static int keyboard_buffer_head=0;
static int keyboard_buffer_tail=0;
static BOOL shift_pressed=false;
static BOOL keycode_extended=false;
static BOOL keycode_released=false;
static UINT8 scancode=0;
INTERRUPT void irq1_keyboard(interrupt_frame_t* frame){ 
    (void)frame; 
    unsigned char ch=inb(PS2_DATA);
    scancode=ch;
    if(scancode==0xe0){
        keycode_extended=true;
        goto kb_done;
    }else if(scancode==0xf0){
        keycode_released=true;
        goto kb_done;
    }
    // now translate into ascii
    if(keycode_released||keycode_extended){
        // extended or released key, we don't handle it for now
        goto kb_clear;
    }
    const ps2_key_code* key=ps2_to_ascii(scancode);
    if(key){
        if(keyboard_buffer_tail+1!=keyboard_buffer_head){
            if(shift_pressed){
                keyboard_buffer[keyboard_buffer_tail]=key->shift_ascii;
            }else{
                keyboard_buffer[keyboard_buffer_tail]=key->ascii;
            }
            char chara=keyboard_buffer[keyboard_buffer_tail];
            char temp_display[2]={chara,'\0'};
            print(temp_display, COLOR_GRAY);
            keyboard_buffer_tail=(keyboard_buffer_tail+1)%KEYBOARD_BUFFER_SIZE;
        }
    }
kb_clear:
    keycode_extended=false;
    keycode_released=false;
kb_done:
    idt_pic_eoi(); 
}


static const ps2_key_code ps2_scan_code_set_2[]={
    {0x1c, 'a', 'A'},
    {0x32, 'b', 'B'},
    {0x21, 'c', 'C'},
    {0x23, 'd', 'D'},
    {0x24, 'e', 'E'},
    {0x2b, 'f', 'F'},
    {0x34, 'g', 'G'},
    {0x33, 'h', 'H'},
    {0x43, 'i', 'I'},
    {0x3b, 'j', 'J'},
    {0x42, 'k', 'K'},
    {0x4b, 'l', 'L'},
    {0x3a, 'm', 'M'},
    {0x31, 'n', 'N'},
    {0x44, 'o', 'O'},
    {0x4d, 'p', 'P'},
    {0x15, 'q', 'Q'},
    {0x2d, 'r', 'R'},
    {0x1b, 's', 'S'},
    {0x2c, 't', 'T'},
    {0x3c, 'u', 'U'},
    {0x2a, 'v', 'V'},
    {0x1d, 'w', 'W'},
    {0x22, 'x', 'X'},
    {0x35, 'y', 'Y'},
    {0x1a, 'z', 'Z'},
    {0x16, '1', '!'},
    {0x1e, '2', '@'},
    {0x26, '3', '#'},
    {0x25, '4', '$'},
    {0x2e, '5', '%'},
    {0x36, '6', '^'},
    {0x3d, '7', '&'},
    {0x3e, '8', '*'},
    {0x46, '9', '('},
    {0x45, '0', ')'},
    {0x0e, '`', '~'},
    {0x4e, '-', '_'},
    {0x55, '=', '+'},
    {0x5d, '\\', '|'},
    {0x54, '[', '{'},
    {0x5b, ']', '}'},
    {0x4c, ';', ':'},
    {0x52, '\'', '"'},
    {0x41, ',', '<'},
    {0x49, '.', '>'},
    {0x4a, '/', '?'},
    {0x29, ' ', ' '},
    {0x0d, '\t', '\t'},
    {0x5a, '\n', '\n'},
    {0x66, '\b', '\b'},
    {0x76, 0x1b, 0x1b},
    {0x70, '0', '0'},
    {0x69, '1', '1'},
    {0x72, '2', '2'},
    {0x7a, '3', '3'},
    {0x6b, '4', '4'},
    {0x73, '5', '5'},
    {0x74, '6', '6'},
    {0x6c, '7', '7'},
    {0x75, '8', '8'},
    {0x7d, '9', '9'},
    {0x71, '.', '.'},
    {0x79, '+', '+'},
    {0x7b, '-', '-'},
    {0x7c, '*', '*'},
};

const ps2_key_code* ps2_to_ascii(UINT8 scan_code){
    UINT32 count=sizeof(ps2_scan_code_set_2)/sizeof(ps2_scan_code_set_2[0]);
    for(UINT32 i=0;i<count;i++){
        if(ps2_scan_code_set_2[i].scan_code==scan_code){
            return &ps2_scan_code_set_2[i];
        }
    }
    return 0;
}
