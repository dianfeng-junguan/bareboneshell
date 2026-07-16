#include "types.h"
#pragma pack(1)
typedef struct{
    UINT16 offse_low;
    UINT16 segment_selector;
    UINT8 rsvd;
    UINT8 flags;
    UINT16 offset_high;
}idt_gate_descriptor32;

typedef struct{
    UINT16 size;
    UINT32 idt;
}idt_idtr;

typedef struct {
  UINT32 eip;
  UINT32 cs;
  UINT32 eflags;
  UINT32 esp;
  UINT32 ss;
} interrupt_frame_t;

#pragma pack()

#define IDT_GATE_TASK 0x5
#define IDT_GATE_INT16 0x6
#define IDT_GATE_TRAP16 0x7
#define IDT_GATE_INT32 0xe
#define IDT_GATE_TRAP32 0xf

#define IDT_DPL_0 0
#define IDT_DPL_1 0x20
#define IDT_DPL_2 0x40
#define IDT_DPL_3 0x60

#define IDT_PRESENT 0x80

#define IDT_FLAG_INT32 0x8e
#define IDT_FLAG_TRAP32 0x8f
#define IDT_FLAG_TASK 0x85
void idt_fill_descriptor(idt_gate_descriptor32* desc,UINT32 offset,UINT16 selector, UINT8 flags);
void idt_set_idt_entry(int index, idt_gate_descriptor32 desc);
void idt_init_8259();
void idt_init();