#include "idt.h"
#include "keyboard.h"
#include "shell.h"
#include "types.h"
#include "vga.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_IRQ_VECTOR 33

static const char scancode_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0
};

static void keyboard_interrupt(u32 vector)
{
    (void)vector;
    u8 status = inb(KEYBOARD_STATUS_PORT);
    if ((status & 0x01) == 0) {
        return;
    }

    u8 scancode = inb(KEYBOARD_DATA_PORT);
    if (scancode & 0x80) {
        return;
    }

    char c = scancode_ascii[scancode];
    if (c) {
        shell_handle_char(c);
    } else {
        vga_write("\n[KBD ] Unknown scan code: ");
        vga_write_hex(scancode);
        vga_write("\nminios> ");
    }
}

void keyboard_init(void)
{
    idt_register_handler(KEYBOARD_IRQ_VECTOR, keyboard_interrupt);
}
