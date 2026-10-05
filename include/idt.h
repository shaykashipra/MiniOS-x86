#ifndef MINIOS_IDT_H
#define MINIOS_IDT_H

#include "types.h"

typedef void (*interrupt_callback_t)(u32 vector);

void idt_init(void);
void idt_register_handler(u8 vector, interrupt_callback_t handler);

#endif
