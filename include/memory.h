#ifndef MINIOS_MEMORY_H
#define MINIOS_MEMORY_H

#include "types.h"

void memory_init(void);
void *memory_alloc(size_t size, size_t alignment);
void memory_free(void *ptr);
void memory_print_state(void);

#endif
