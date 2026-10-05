#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "memory.h"
#include "shell.h"
#include "types.h"
#include "vga.h"

void kernel_main(void)
{
    vga_init();

    vga_write("================================\n");
    vga_write("MiniOS x86\n");
    vga_write("==========\n\n");
    vga_write("[BOOT] Bootloader initialized\n");
    vga_write("[CPU ] Protected mode enabled\n");

    gdt_init();
    vga_write("[GDT ] GDT loaded\n");
    vga_write("[VGA ] Display initialized\n");

    memory_init();
    vga_write("[MEM ] Memory manager initialized\n");

    idt_init();
    vga_write("[IDT ] Interrupt system initialized\n");

    keyboard_init();
    vga_write("[KBD ] Keyboard driver initialized\n\n");

    vga_write("MiniOS kernel started successfully.\n\n");
    shell_init();

    enable_interrupts();
    while (1) {
        halt_cpu();
    }
}
