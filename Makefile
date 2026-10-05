AS = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-i386
GDB = gdb

BUILD_DIR = build
BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
IMAGE = $(BUILD_DIR)/minios.img

ASM_OBJS = $(BUILD_DIR)/entry.o $(BUILD_DIR)/isr.o
C_OBJS = \
	$(BUILD_DIR)/kernel.o \
	$(BUILD_DIR)/gdt.o \
	$(BUILD_DIR)/idt.o \
	$(BUILD_DIR)/vga.o \
	$(BUILD_DIR)/keyboard.o \
	$(BUILD_DIR)/memory.o \
	$(BUILD_DIR)/shell.o

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -Iinclude -Wall -Wextra -O2
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

.PHONY: all clean run debug verify check-tools

all: $(IMAGE)

check-tools:
	@command -v $(AS) >/dev/null || { echo "Missing nasm"; exit 1; }
	@command -v $(CC) >/dev/null || { echo "Missing gcc"; exit 1; }
	@command -v $(LD) >/dev/null || { echo "Missing ld"; exit 1; }
	@command -v $(OBJCOPY) >/dev/null || { echo "Missing objcopy"; exit 1; }
	@command -v python3 >/dev/null || { echo "Missing python3"; exit 1; }
	@command -v $(QEMU) >/dev/null || { echo "Missing qemu-system-i386"; exit 1; }

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: boot/entry.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

$(BUILD_DIR)/isr.o: boot/isr.asm | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

$(BUILD_DIR)/kernel.o: kernel/kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gdt.o: kernel/cpu/gdt.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/idt.o: kernel/cpu/idt.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/vga.o: kernel/drivers/vga.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: kernel/drivers/keyboard.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/memory.o: kernel/memory/memory.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/shell.o: kernel/shell.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(ASM_OBJS) $(C_OBJS) linker.ld
	$(LD) $(LDFLAGS) $(ASM_OBJS) $(C_OBJS) -o $@

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

$(BOOT_BIN): boot/boot.asm $(KERNEL_BIN)
	$(AS) -f bin -DKERNEL_SECTORS=$(shell python3 -c 'import os, math; print(max(1, math.ceil(os.path.getsize("$(KERNEL_BIN)") / 512)))') $< -o $@

$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(IMAGE)
	@python3 -c 'import os; p="$(IMAGE)"; s=os.path.getsize(p); pad=(512-(s%512))%512; open(p,"ab").write(bytes(pad))'
	@echo "Created $(IMAGE)"

run: $(IMAGE)
	$(QEMU) -drive format=raw,file=$(IMAGE)

debug: $(IMAGE) $(KERNEL_ELF)
	$(QEMU) -drive format=raw,file=$(IMAGE) -S -s

verify: $(IMAGE)
	@python3 -c 'from pathlib import Path; b=Path("$(BOOT_BIN)").read_bytes(); assert len(b)==512, len(b); assert b[510:512]==b"\x55\xaa", b[510:512]; assert Path("$(IMAGE)").stat().st_size >= 1024; print("Verified boot sector size, boot signature, and disk image creation.")'

clean:
	rm -rf $(BUILD_DIR)
