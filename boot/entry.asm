; Second-stage entry code. It starts in 16-bit real mode at 0x8000,
; enables protected mode, then calls the freestanding C kernel.

BITS 16
GLOBAL _start
EXTERN kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

SECTION .text
_start:
    cli
    call enable_a20
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:protected_entry

enable_a20:
    ; Fast A20 gate. QEMU supports it and it keeps this project readable.
    in al, 0x92
    or al, 00000010b
    out 0x92, al
    ret

BITS 32
protected_entry:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, stack_top

    call kernel_main

.halt:
    hlt
    jmp .halt

ALIGN 8
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF     ; base 0, limit 4 GiB, code, ring 0
    dq 0x00CF92000000FFFF     ; base 0, limit 4 GiB, data, ring 0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

SECTION .bss
ALIGN 16
stack_bottom:
    resb 16384
stack_top:
