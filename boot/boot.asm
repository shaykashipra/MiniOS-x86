; MiniOS-x86 boot sector
; BIOS loads this 512-byte sector at physical address 0x7C00.

BITS 16
ORG 0x7C00

%ifndef KERNEL_SECTORS
%define KERNEL_SECTORS 64
%endif

KERNEL_LOAD_SEG equ 0x0000
KERNEL_LOAD_OFF equ 0x8000

start:
    jmp 0x0000:boot_start

boot_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl
    mov si, boot_msg
    call print_string

    mov ax, KERNEL_LOAD_SEG
    mov es, ax
    mov bx, KERNEL_LOAD_OFF
    mov ah, 0x02              ; BIOS disk read
    mov al, KERNEL_SECTORS    ; sectors to read after the boot sector
    mov ch, 0x00              ; cylinder 0
    mov cl, 0x02              ; sector 2 (sector numbers start at 1)
    mov dh, 0x00              ; head 0
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    cmp al, KERNEL_SECTORS
    jne disk_error

    mov si, load_msg
    call print_string
    jmp KERNEL_LOAD_SEG:KERNEL_LOAD_OFF

disk_error:
    mov si, disk_error_msg
    call print_string
    cli
.hang:
    hlt
    jmp .hang

print_string:
    pusha
.next:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E              ; BIOS teletype output
    mov bh, 0x00
    mov bl, 0x07
    int 0x10
    jmp .next
.done:
    popa
    ret

boot_drive db 0
boot_msg db 13, 10, "MiniOS bootloader: reading kernel...", 13, 10, 0
load_msg db "Kernel loaded. Entering stage 2.", 13, 10, 0
disk_error_msg db "Disk read failed. System halted.", 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xAA55
