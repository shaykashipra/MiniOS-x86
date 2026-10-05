#include "idt.h"
#include "keyboard.h"
#include "types.h"
#include "vga.h"

#define IDT_ENTRIES 256
#define KERNEL_CODE_SEGMENT 0x08

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

struct idt_entry {
    u16 base_low;
    u16 selector;
    u8 zero;
    u8 flags;
    u16 base_high;
} __attribute__((packed));

struct idt_pointer {
    u16 limit;
    u32 base;
} __attribute__((packed));

extern void idt_load(struct idt_pointer *ptr);
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void irq0(void);
extern void irq1(void);

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_pointer idt_ptr;
static interrupt_callback_t callbacks[IDT_ENTRIES];

static void idt_set_gate(u8 vector, u32 base, u16 selector, u8 flags)
{
    idt[vector].base_low = (u16)(base & 0xFFFF);
    idt[vector].base_high = (u16)((base >> 16) & 0xFFFF);
    idt[vector].selector = selector;
    idt[vector].zero = 0;
    idt[vector].flags = flags;
}

static void pic_remap(void)
{
    u8 master_mask = inb(PIC1_DATA);
    u8 slave_mask = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();
    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    outb(PIC1_DATA, master_mask);
    outb(PIC2_DATA, slave_mask);
}

static void pic_enable_keyboard_only(void)
{
    /* Enable IRQ0 and IRQ1 on the master PIC; mask all slave IRQs. */
    outb(PIC1_DATA, 0xFC);
    outb(PIC2_DATA, 0xFF);
}

void idt_register_handler(u8 vector, interrupt_callback_t handler)
{
    callbacks[vector] = handler;
}

void isr_dispatch(u32 vector)
{
    if (callbacks[vector]) {
        callbacks[vector](vector);
    } else if (vector < 32) {
        vga_write("\n[INT ] Unexpected CPU exception: ");
        vga_write_dec(vector);
        vga_write("\n");
    }

    if (vector >= 32 && vector < 48) {
        if (vector >= 40) {
            outb(PIC2_COMMAND, PIC_EOI);
        }
        outb(PIC1_COMMAND, PIC_EOI);
    }
}

void idt_init(void)
{
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (u32)&idt;

    for (u32 i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate((u8)i, 0, KERNEL_CODE_SEGMENT, 0x8E);
        callbacks[i] = NULL;
    }

    idt_set_gate(0, (u32)isr0, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(1, (u32)isr1, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(2, (u32)isr2, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(3, (u32)isr3, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(4, (u32)isr4, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(5, (u32)isr5, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(6, (u32)isr6, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(7, (u32)isr7, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(8, (u32)isr8, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(9, (u32)isr9, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(10, (u32)isr10, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(11, (u32)isr11, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(12, (u32)isr12, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(13, (u32)isr13, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(14, (u32)isr14, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(15, (u32)isr15, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(16, (u32)isr16, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(17, (u32)isr17, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(18, (u32)isr18, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(19, (u32)isr19, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(20, (u32)isr20, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(21, (u32)isr21, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(22, (u32)isr22, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(23, (u32)isr23, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(24, (u32)isr24, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(25, (u32)isr25, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(26, (u32)isr26, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(27, (u32)isr27, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(28, (u32)isr28, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(29, (u32)isr29, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(30, (u32)isr30, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(31, (u32)isr31, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(32, (u32)irq0, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(33, (u32)irq1, KERNEL_CODE_SEGMENT, 0x8E);

    pic_remap();
    pic_enable_keyboard_only();
    idt_load(&idt_ptr);
}
