#include "memory.h"
#include "shell.h"
#include "types.h"
#include "vga.h"

#define COMMAND_BUFFER_SIZE 64

static char command_buffer[COMMAND_BUFFER_SIZE];
static u32 command_length;

static int strings_equal(const char *left, const char *right)
{
    while (*left && *right && *left == *right) {
        left++;
        right++;
    }
    return *left == '\0' && *right == '\0';
}

static void print_prompt(void)
{
    vga_write("minios> ");
}

static void run_command(const char *command)
{
    if (strings_equal(command, "help")) {
        vga_write("Available commands:\nhelp\nclear\nabout\nmem\ncpu\n");
    } else if (strings_equal(command, "clear")) {
        vga_clear();
    } else if (strings_equal(command, "about")) {
        vga_write("MiniOS-x86: educational 32-bit x86 bootloader and tiny C kernel.\n");
    } else if (strings_equal(command, "mem")) {
        memory_print_state();
    } else if (strings_equal(command, "cpu")) {
        shell_print_cpu();
    } else if (command[0] != '\0') {
        vga_write("Unknown command: ");
        vga_write(command);
        vga_write("\n");
    }
}

void shell_print_cpu(void)
{
    vga_write("CPU mode: 32-bit protected mode\n");
    vga_write("Segments: flat kernel code/data selectors 0x08/0x10\n");
    vga_write("Interrupts: IDT installed, PIC remapped to 0x20-0x2F\n");
}

void shell_init(void)
{
    command_length = 0;
    print_prompt();
}

void shell_handle_char(char c)
{
    if (c == '\n') {
        vga_put_char('\n');
        command_buffer[command_length] = '\0';
        run_command(command_buffer);
        command_length = 0;
        print_prompt();
        return;
    }

    if (c == '\b') {
        if (command_length > 0) {
            command_length--;
            vga_put_char('\b');
        }
        return;
    }

    if (command_length < COMMAND_BUFFER_SIZE - 1) {
        command_buffer[command_length++] = c;
        vga_put_char(c);
    }
}
