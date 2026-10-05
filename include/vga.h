#ifndef MINIOS_VGA_H
#define MINIOS_VGA_H

#include "types.h"

void vga_init(void);
void vga_clear(void);
void vga_put_char(char c);
void vga_write(const char *text);
void vga_write_hex(u32 value);
void vga_write_dec(u32 value);

#endif
