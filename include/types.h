#ifndef MINIOS_TYPES_H
#define MINIOS_TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned int size_t;

#define NULL ((void *)0)

static inline void outb(u16 port, u8 value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline u8 inb(u16 port)
{
    u8 value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_wait(void)
{
    outb(0x80, 0);
}

static inline void enable_interrupts(void)
{
    __asm__ volatile ("sti");
}

static inline void halt_cpu(void)
{
    __asm__ volatile ("hlt");
}

#endif
