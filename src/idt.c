#include "idt.h"
#include "types.h"
#include "ins.h"


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI 0x20

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define CASCADE_IRQ 2

void idt_init_pic(){
    // remap the PICs to avoid conflicts with CPU exceptions
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, 0x20);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, 0x28);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 1 << CASCADE_IRQ);        // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	io_wait();
	outb(PIC2_DATA, CASCADE_IRQ);             // ICW3: tell Slave PIC its cascade identity
	io_wait();
	
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// Unmask both PICs.
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}
void idt_pic_eoi(){
    outb(PIC1_COMMAND, PIC_EOI);
    outb(PIC2_COMMAND, PIC_EOI);
}
void idt_pic_disable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}
void idt_pic_set_mask(UINT8 bitloc) {
    UINT16 port;
    UINT8 value;

    if(bitloc < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        bitloc -= 8;
    }
    value = inb(port) | (1 << bitloc);
    outb(port, value);        
}

void idt_pic_clear_mask(UINT8 bitloc) {
    UINT16 port;
    UINT8 value;

    if(bitloc < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        bitloc -= 8;
    }
    value = inb(port) & ~(1 << bitloc);
    outb(port, value);        
}

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

INTERRUPT void isr0_divide(interrupt_frame_t* frame){
    (void)frame;
    for(;;);
}
INTERRUPT void isr1_debug(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr2_nmi(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr3_breakpoint(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr4_overflow(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr5_bound(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr6_invalid_opcode(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr7_device_not_available(interrupt_frame_t* frame){ (void)frame; for(;;); }

/* Exceptions that push an error code */
INTERRUPT void isr8_double_fault(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }

INTERRUPT void isr9_coprocessor_overrun(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr10_invalid_tss(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
INTERRUPT void isr11_segment_not_present(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
INTERRUPT void isr12_stack_segment_fault(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
INTERRUPT void isr13_general_protection(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
INTERRUPT void isr14_page_fault(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }

INTERRUPT void isr15_reserved(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr16_fpu(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr17_alignment_check(interrupt_frame_t* frame, UINT32 err){ (void)frame; (void)err; for(;;); }
INTERRUPT void isr18_machine_check(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr19_simd_fp(interrupt_frame_t* frame){ (void)frame; for(;;); }
INTERRUPT void isr20_virtualization(interrupt_frame_t* frame){ (void)frame; for(;;); }

// pic handlers

INTERRUPT void irq0_timer(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
extern INTERRUPT void irq1_keyboard(interrupt_frame_t* frame);
INTERRUPT void irq2_cascade(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq3_serial2(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq4_serial1(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq5_parallel2(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq6_floppy(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq7_parallel1(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq8_rtc(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq9_legacy(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq10_reserved(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq11_reserved(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq12_ps2_mouse(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq13_fpu(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq14_primary_ata(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }
INTERRUPT void irq15_secondary_ata(interrupt_frame_t* frame){ (void)frame; idt_pic_eoi(); }

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

    /* register PIC IRQ handlers at vectors 32..47 */
    idt_fill_descriptor(&idt[32], (UINT32)irq0_timer,                  0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[33], (UINT32)irq1_keyboard,               0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[34], (UINT32)irq2_cascade,                0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[35], (UINT32)irq3_serial2,                0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[36], (UINT32)irq4_serial1,                0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[37], (UINT32)irq5_parallel2,              0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[38], (UINT32)irq6_floppy,                 0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[39], (UINT32)irq7_parallel1,              0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[40], (UINT32)irq8_rtc,                    0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[41], (UINT32)irq9_legacy,                 0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[42], (UINT32)irq10_reserved,              0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[43], (UINT32)irq11_reserved,              0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[44], (UINT32)irq12_ps2_mouse,             0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[45], (UINT32)irq13_fpu,                   0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[46], (UINT32)irq14_primary_ata,           0x8, IDT_FLAG_INT32);
    idt_fill_descriptor(&idt[47], (UINT32)irq15_secondary_ata,         0x8, IDT_FLAG_INT32);

    // load idt
    __asm__ volatile("lidt %0"::"m"(idtr));
    // config 8259 PIC
    idt_init_pic();

    __asm__ volatile("sti");
}