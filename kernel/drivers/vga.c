#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile u16 *)0xB8000)
#define VGA_COLOR 0x0F

static u8 row;
static u8 column;

static u16 make_cell(char c)
{
    return ((u16)VGA_COLOR << 8) | (u8)c;
}

static void move_to_next_line(void)
{
    column = 0;
    if (row < VGA_HEIGHT - 1) {
        row++;
        return;
    }

    for (u8 y = 1; y < VGA_HEIGHT; y++) {
        for (u8 x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }

    for (u8 x = 0; x < VGA_WIDTH; x++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = make_cell(' ');
    }
}

void vga_clear(void)
{
    for (u8 y = 0; y < VGA_HEIGHT; y++) {
        for (u8 x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = make_cell(' ');
        }
    }
    row = 0;
    column = 0;
}

void vga_init(void)
{
    vga_clear();
}

void vga_put_char(char c)
{
    if (c == '\n') {
        move_to_next_line();
        return;
    }

    if (c == '\b') {
        if (column > 0) {
            column--;
            VGA_MEMORY[row * VGA_WIDTH + column] = make_cell(' ');
        }
        return;
    }

    VGA_MEMORY[row * VGA_WIDTH + column] = make_cell(c);
    column++;

    if (column >= VGA_WIDTH) {
        move_to_next_line();
    }
}

void vga_write(const char *text)
{
    while (*text) {
        vga_put_char(*text++);
    }
}

void vga_write_hex(u32 value)
{
    const char *digits = "0123456789ABCDEF";
    vga_write("0x");
    for (i32 shift = 28; shift >= 0; shift -= 4) {
        vga_put_char(digits[(value >> shift) & 0xF]);
    }
}

void vga_write_dec(u32 value)
{
    char buffer[11];
    u8 index = 0;

    if (value == 0) {
        vga_put_char('0');
        return;
    }

    while (value > 0 && index < sizeof(buffer)) {
        buffer[index++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (index > 0) {
        vga_put_char(buffer[--index]);
    }
}
