#define VIDEO_BUFFER_BASE 0xb8000ul
#define VIDEO_LINE_CHARS 80
#define VIDEO_LINES 25
static int cursor_x=0,cursor_y=0;

#define COLOR_GRAY 0x7
#define COLOR_RED 0xc
#define COLOR_BLUE 0x1
#define COLOR_GREEN 0xa
#define COLOR_LIGHTBLUE 0xb
void set_cursor(int x,int y){
    cursor_x=x;
    cursor_y=y;
}
void print(char* str,char color){
    char* ptr=(char*)(VIDEO_BUFFER_BASE+(cursor_y*VIDEO_LINE_CHARS+cursor_x)*2);
    while (*str) {
        // special chars
        if (*str=='\n') {
            set_cursor(0, cursor_y+1);
            ptr=(char*)(VIDEO_BUFFER_BASE+(cursor_y*VIDEO_LINE_CHARS+cursor_x)*2);
            str++;
        }else if (*str=='\t') {
            for (int i=0; i<4-cursor_x%4; i++) {
                *ptr++=' ';
                *ptr++=0;
            }
            set_cursor(cursor_x+4-cursor_x%4, cursor_y);
            ptr=(char*)(VIDEO_BUFFER_BASE+(cursor_y*VIDEO_LINE_CHARS+cursor_x)*2);
            str++;
        }else{
            *ptr++=*str++;
            *ptr++=color;
            cursor_x++;
            if (cursor_x>=VIDEO_LINE_CHARS) {
                cursor_x=0;
                cursor_y++;
            }

        }
    }
}
#define bool int
#define true 1
#define false 0
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
bool ps2_has_two_channels=false;
int ps2_init(){
    ps2_disable();
    ps2_flush_output();
    unsigned char config=ps2_read_configure_byte();
    config&=0xad;
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
    return 0;
}
#define PANIC(msg) do{print("PANIC: " msg "\n", COLOR_RED);while(1);}while(0)
int main(){
    // __asm__ volatile("sti");
    print("bareshell c main running\n", COLOR_GRAY);
    print("test\ttab\ta\n", COLOR_GRAY);
    // PS/2 键盘初始化
    if(ps2_init()!=0){
        PANIC("ps2 init failed");
    }
    print("ps2 init success\n", COLOR_GREEN);
    while(1);
}