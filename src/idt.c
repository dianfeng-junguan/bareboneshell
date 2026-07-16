#include "idt.h"
#include "types.h"

void idt_fill_descriptor(idt_gate_descriptor32* desc,UINT32 offset,UINT16 selector, UINT8 flags){
    desc->offse_low=offset&0xffff;
    desc->segment_selector=selector;
    desc->rsvd = 0;
    desc->flags=flags;
    desc->offset_high=(offset>>16)&0xffff;
}
extern idt_gate_descriptor32 idt[256];
extern idt_idtr idtr;
void idt_set_idt_entry(int index, idt_gate_descriptor32 desc){
    idt[index]=desc;
}

/* Exception handlers: a minimal set for CPU exceptions 0..20.
   Use GCC's interrupt attribute so compiler generates proper IRET. */

__attribute__((interrupt)) void isr0_divide(interrupt_frame_t* frame){
    (void)frame;
    for(;;);
}
__attribute__((interrupt)) void isr1_debug(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr2_nmi(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr3_breakpoint(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr4_overflow(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr5_bound(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr6_invalid_opcode(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr7_device_not_available(interrupt_frame_t* frame){ (void)frame; for(;;); }

/* Exceptions that push an error code */
__attribute__((interrupt)) void isr8_double_fault(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }

__attribute__((interrupt)) void isr9_coprocessor_overrun(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr10_invalid_tss(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
__attribute__((interrupt)) void isr11_segment_not_present(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
__attribute__((interrupt)) void isr12_stack_segment_fault(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
__attribute__((interrupt)) void isr13_general_protection(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
__attribute__((interrupt)) void isr14_page_fault(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }

__attribute__((interrupt)) void isr15_reserved(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr16_fpu(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr17_alignment_check(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
__attribute__((interrupt)) void isr18_machine_check(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr19_simd_fp(interrupt_frame_t* frame){ (void)frame; for(;;); }
__attribute__((interrupt)) void isr20_virtualization(interrupt_frame_t* frame){ (void)frame; for(;;); }

void idt_init(){
    /* register handlers 0..20 */
    idt_fill_descriptor(&idt[0],  (UINT32)isr0_divide,                 0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[1],  (UINT32)isr1_debug,                  0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[2],  (UINT32)isr2_nmi,                    0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[3],  (UINT32)isr3_breakpoint,             0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[4],  (UINT32)isr4_overflow,               0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[5],  (UINT32)isr5_bound,                  0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[6],  (UINT32)isr6_invalid_opcode,         0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[7],  (UINT32)isr7_device_not_available,  0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[8],  (UINT32)isr8_double_fault,           0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[9],  (UINT32)isr9_coprocessor_overrun,    0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[10], (UINT32)isr10_invalid_tss,           0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[11], (UINT32)isr11_segment_not_present,  0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[12], (UINT32)isr12_stack_segment_fault,  0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[13], (UINT32)isr13_general_protection,   0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[14], (UINT32)isr14_page_fault,           0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[15], (UINT32)isr15_reserved,             0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[16], (UINT32)isr16_fpu,                  0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[17], (UINT32)isr17_alignment_check,      0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[18], (UINT32)isr18_machine_check,        0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[19], (UINT32)isr19_simd_fp,              0x8, IDT_FLAG_TRAP32);
    idt_fill_descriptor(&idt[20], (UINT32)isr20_virtualization,       0x8, IDT_FLAG_TRAP32);

    // load idt
    __asm__ volatile("lidt %0"::"a"(&idtr));
    // config 8259 PIC
    idt_init_8259();
    __asm__ volatile("sti");
}
void idt_init_8259(){
    
}