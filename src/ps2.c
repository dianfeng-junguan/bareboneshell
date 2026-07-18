#include "ps2.h"
#include "print.h"
bool ps2_has_two_channels=false;
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
INTERRUPT void irq1_keyboard(interrupt_frame_t* frame){ 
    (void)frame; 
    unsigned char ch=inb(PS2_DATA);
    print("Keyboard interrupt received\n", COLOR_GREEN);
    idt_pic_eoi(); 
}
