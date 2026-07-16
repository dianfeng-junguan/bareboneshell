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

int main(){
    __asm__ volatile("sti");
    print("bareshell c main running\n", COLOR_GRAY);
    print("test\ttab\ta", COLOR_GRAY);
    while(1);
}