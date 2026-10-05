#include "memory.h"
#include "vga.h"

#define HEAP_START 0x00100000
#define HEAP_SIZE  (64 * 1024)

static u32 heap_start;
static u32 heap_end;
static u32 next_free;
static u32 allocation_count;

static u32 align_up(u32 value, u32 alignment)
{
    if (alignment == 0) {
        return value;
    }
    return (value + alignment - 1) & ~(alignment - 1);
}

void memory_init(void)
{
    heap_start = HEAP_START;
    heap_end = HEAP_START + HEAP_SIZE;
    next_free = heap_start;
    allocation_count = 0;
}

void *memory_alloc(size_t size, size_t alignment)
{
    u32 aligned = align_up(next_free, alignment);
    u32 new_next = aligned + size;

    if (new_next > heap_end) {
        return NULL;
    }

    next_free = new_next;
    allocation_count++;
    return (void *)aligned;
}

void memory_free(void *ptr)
{
    (void)ptr;
    /* This educational bump allocator cannot reclaim individual blocks. */
}

void memory_print_state(void)
{
    vga_write("Heap start: ");
    vga_write_hex(heap_start);
    vga_write("\nHeap end:   ");
    vga_write_hex(heap_end);
    vga_write("\nNext free:  ");
    vga_write_hex(next_free);
    vga_write("\nUsed bytes: ");
    vga_write_dec(next_free - heap_start);
    vga_write("\nAllocations:");
    vga_write_dec(allocation_count);
    vga_write("\n");
}
