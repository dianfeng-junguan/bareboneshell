#include "print.h"
#include "idt.h"
static int cursor_x=0,cursor_y=0;
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
#define PANIC(msg) do{print("PANIC: " msg "\n", COLOR_RED);while(1);}while(0)
#include "ps2.h"
int main(){
    print("bareshell c main running\n", COLOR_GRAY);
    print("test\ttab\ta\n", COLOR_GRAY);
    // init idt
    idt_init();
    // PS/2 init
    if(ps2_init()!=0){
        PANIC("ps2 init failed");
    }
    print("ps2 init success\n", COLOR_GREEN);
    while(1){
    }
}