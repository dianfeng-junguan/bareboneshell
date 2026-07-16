#include "types.h"
extern void ps2_send_command(unsigned char command);
extern unsigned char ps2_read_configure_byte();
extern void ps2_write_configure_byte(unsigned char configure_byte);
extern int ps2_self_test();
extern bool ps2_check_2_channels_and_disable();
extern int ps2_interface_test();
extern void ps2_enable_first();
extern void ps2_enable_second();
extern void ps2_disable();
extern void ps2_flush_output();
extern int ps2_reset_device();
int ps2_init();