#include "types.h"

static inline void outb(UINT16 port, UINT8 value) {
    __asm__ volatile("outb %0, %1"::"a"(value), "d"(port));
}
static inline void outw(UINT16 port, UINT16 value) {
    __asm__ volatile("outw %0, %1"::"a"(value), "d"(port));
}
static inline UINT8 inb(UINT16 port) {
    UINT8 ret;
    __asm__ volatile("inb %1, %0":"=a"(ret):"d"(port));
    return ret;
}
static inline UINT16 inw(UINT16 port) {
    UINT16 ret;
    __asm__ volatile("inw %1, %0":"=a"(ret):"d"(port));
    return ret;
}

static inline void io_wait() {
    __asm__ volatile("nop\r\n nop\r\n nop\r\n nop\r\n");
}
