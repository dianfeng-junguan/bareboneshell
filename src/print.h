#define VIDEO_BUFFER_BASE 0xb8000ul
#define VIDEO_LINE_CHARS 80
#define VIDEO_LINES 25

#define COLOR_GRAY 0x7
#define COLOR_RED 0xc
#define COLOR_BLUE 0x1
#define COLOR_GREEN 0xa
#define COLOR_LIGHTBLUE 0xb
void set_cursor(int x,int y);
void print(char* str,char color);