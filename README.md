# MiniOS-x86

MiniOS-x86 is a small educational x86 operating-system environment that boots in QEMU, enters 32-bit protected mode, and runs a freestanding C kernel. The project is designed for learning and interview discussion, not for production use.

## Motivation

The goal is to demonstrate low-level software fundamentals in a project that is small enough to explain clearly: BIOS booting, Assembly startup code, C in a freestanding environment, descriptor tables, interrupts, simple device I/O, memory concepts, Make-based builds, QEMU, and GDB.

## Features

- 512-byte BIOS boot sector with `0xAA55` signature
- BIOS disk read of a flat second-stage/kernel image
- 16-bit real-mode entry code that enables A20 and switches to protected mode
- Minimal GDT with null, kernel code, and kernel data descriptors
- Freestanding 32-bit C kernel
- VGA text-mode output using memory at `0xB8000`
- Minimal IDT and PIC remapping
- Basic PS/2 keyboard interrupt input
- Simple bump allocator diagnostic
- Tiny shell with `help`, `clear`, `about`, `mem`, and `cpu`
- QEMU run target and GDB-friendly ELF output

## Architecture

```text
BIOS
  |
  v
boot/boot.asm        512-byte boot sector at 0x7C00
  |
  | reads kernel image from disk sectors into 0x8000
  v
boot/entry.asm       real mode -> protected mode transition
  |
  v
kernel/kernel.c      freestanding C kernel
  |
  +--> kernel/cpu/       GDT and IDT setup
  +--> kernel/drivers/   VGA and keyboard drivers
  +--> kernel/memory/    educational bump allocator
  +--> kernel/shell.c    small command shell
```

## Boot Flow

1. The BIOS loads the first disk sector to physical address `0x7C00`.
2. The boot sector prints a message using BIOS interrupt `0x10`.
3. The boot sector reads the flat kernel image from disk starting at sector 2.
4. Control jumps to `0x8000`, where `boot/entry.asm` starts.
5. The entry code enables A20, loads an early GDT, sets the protected-mode bit in `CR0`, and far-jumps into 32-bit code.
6. The C kernel initializes VGA, GDT, memory state, IDT, keyboard input, and the shell.

## Repository Structure

```text
boot/        Boot sector, protected-mode entry, interrupt stubs
kernel/      C kernel code
include/     Kernel headers
libc/        Reserved for future freestanding helper code
docs/        Private interview notes, ignored by Git
build/       Generated objects, ELF, binary, and disk image
linker.ld    Kernel linker script
Makefile     Build, run, debug, and clean targets
```

## Technologies

- C
- x86 Assembly with NASM syntax
- GNU GCC and binutils
- Make
- QEMU
- GDB

## Requirements

On Ubuntu or WSL Ubuntu:

```sh
sudo apt update
sudo apt install build-essential nasm qemu-system-x86 gdb
```

The project expects these commands to exist:

```sh
gcc
nasm
ld
objcopy
make
qemu-system-i386
gdb
python3
```

## Build

```sh
make
```

This creates:

- `build/boot.bin`: 512-byte boot sector
- `build/kernel.elf`: ELF file useful for GDB
- `build/kernel.bin`: flat kernel image loaded by the boot sector
- `build/minios.img`: bootable raw disk image

Important flags:

- `-m32`: builds 32-bit x86 code
- `-ffreestanding`: tells GCC there is no hosted C runtime
- `-nostdlib`: avoids linking the host operating system's standard library
- `-nostdinc`: avoids accidental dependency on host C headers
- `-fno-stack-protector`: avoids compiler-generated runtime calls not available in this kernel

## Run

```sh
make run
```

If graphical QEMU is inconvenient under WSL, run a direct command with a display option suitable for your environment, for example:

```sh
qemu-system-i386 -drive format=raw,file=build/minios.img -display curses
```

## Debug

Start QEMU paused with a GDB server:

```sh
make debug
```

In another terminal:

```sh
gdb build/kernel.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
(gdb) info registers
(gdb) x/16x $esp
(gdb) disassemble
(gdb) stepi
```

## Checking Workflow

Use this sequence while developing:

```sh
make clean
make
make verify
make run
```

`make verify` checks the generated boot sector size, boot signature, and disk image creation. QEMU boot, keyboard input, shell commands, VGA output, GDT setup, and IDT setup are still runtime checks that should be verified with `make run` and, when needed, GDB.

## Example Output

```text
================================
MiniOS x86
==========

[BOOT] Bootloader initialized
[CPU ] Protected mode enabled
[GDT ] GDT loaded
[VGA ] Display initialized
[MEM ] Memory manager initialized
[IDT ] Interrupt system initialized
[KBD ] Keyboard driver initialized

MiniOS kernel started successfully.

minios>
```

## Technical Concepts Demonstrated

- BIOS boot sector layout and boot signature
- Disk sector loading through BIOS services
- Real mode and protected mode
- GDT and segment selectors
- Freestanding C compilation
- Linker scripts and fixed load addresses
- VGA memory-mapped text output
- IDT interrupt gates
- PIC remapping and end-of-interrupt signaling
- PS/2 keyboard scan-code input
- Simple memory allocator state
- QEMU and GDB low-level debugging

## Limitations

- This is an educational kernel, not a full operating system.
- There is no filesystem, scheduler, process model, paging, or userspace.
- Keyboard support handles only a small US scan-code subset.
- The allocator is a bump allocator and cannot reclaim individual blocks.
- The bootloader assumes a small flat image loaded from early disk sectors.

## Future Improvements

- Add paging with identity mapping
- Add serial logging for easier headless debugging
- Add a small C string/memory helper library
- Add timer ticks and a simple uptime command
- Add better keyboard handling for Shift and special keys
- Add automated QEMU smoke tests
