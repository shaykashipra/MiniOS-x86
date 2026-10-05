#include "gdt.h"
#include "types.h"

#define GDT_ENTRIES 3

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed));

struct gdt_pointer {
    u16 limit;
    u32 base;
} __attribute__((packed));

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_pointer gdt_ptr;

static void gdt_set_entry(u32 index, u32 base, u32 limit, u8 access, u8 granularity)
{
    gdt[index].base_low = (u16)(base & 0xFFFF);
    gdt[index].base_middle = (u8)((base >> 16) & 0xFF);
    gdt[index].base_high = (u8)((base >> 24) & 0xFF);
    gdt[index].limit_low = (u16)(limit & 0xFFFF);
    gdt[index].granularity = (u8)((limit >> 16) & 0x0F);
    gdt[index].granularity |= granularity & 0xF0;
    gdt[index].access = access;
}

static void gdt_flush(struct gdt_pointer *ptr)
{
    __asm__ volatile (
        "lgdt (%0)\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        "ljmp $0x08, $1f\n"
        "1:\n"
        :
        : "r"(ptr)
        : "ax", "memory"
    );
}

void gdt_init(void)
{
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (u32)&gdt;

    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush(&gdt_ptr);
}
