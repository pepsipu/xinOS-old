;***************************************************************************
; shrineOS bootloader - bootloader.asm
; Sammy Hajhamid
; December 25, 2019
;
; Loads kernel and extra data (such success/error messages and the GDT)
; from the disk as well as sets up memory segments
;***************************************************************************

[org 0x7c00]
[bits 16]

%define DISK 0x13
%define DISPLAY 0x10

%macro print 1
    mov si, %1
    call print_si
%endmacro

; BOOTLOADER - 512 bytes - sector 1
start:
    ; don't modify stack pointer and base pointer, on boot, bp = 0, and sp = to some value in between 0x7c00 (under bootloader) and 0x500 (above bios data)

    ; set es segment to 0x7000
    mov ax, 0x700 ; will be shifted over by 4
    mov es, ax

    ; print confirmation that we have booted
    print ok

    ; read strings/data from disk to memory right above bootloader
    mov bx, 0xe00 ; data/strings location
    mov ah, 0x2 ; (bios) read a sector mode
    mov al, 0x1 ; read 1 sector
    mov cx, 0x2 ; ch = cylinder 0, cl = sector 2
    mov dh, 0x0 ; head 0
    int DISK ; call bios to handle disk operation
    jc disk_fail ; if the carry flag is set (something went wrong) hang

    ; read kernel from disk into memory above strings/data
    mov bx, 0xf00 ; data/strings location
    mov ah, 0x2 ; (bios) read a sector mode
    mov al, 0xa ; read 10 sectors
    mov cx, 0x3 ; ch = cylinder 0, cl = sector 3
    mov dh, 0x0 ; head 0
    int DISK ; call bios to handle disk operation
    jc disk_fail ; if the carry flag is set (something went wrong) hang
    ; print messages
    print load ; just a helpful message
    print disk_okay ; since we could load the strings, we know disk is fine

    ; switch to 32 bit protected mode
    ; first setup the global descriptor table which is defined in the sector we just loaded
    cli ; disable interrupts because when we switch we do not want the BIOS handling our interrupts
    lgdt [gdt_descriptor] ; set addr of gdt descriptor so the cpu knows where our GDT is at
    print gdt_okay

    ; set 32 bit mode in control register
    mov esi, cr0 ; get contents of the control register so we can modify them
    or esi, 0x1 ; set esi's first bit (16 bit or 32 bit mode flag) to 1
    mov cr0, esi ; flag is now set, we go from 16 bit real mode to 32 bit protected mode
    jmp CODE_SEGMENT:start_protected ; make a far jump to flush the CPU pipeline so we don't keep executing 16 bit code
    print 

    ; print finish boot message
    print finish

    jmp hang

print_si:
    mov ah, 0x0e ; (bios) teletype mode
    .loop:
    lodsb ; load byte from si into al and inc si
    test al, al ; check if al is a null byte
    jz .ret ; if it is return
    int DISPLAY ; if not print
    jmp .loop ; then go back to loading another byte
    .ret:
    mov al, 0xa ; newline
    int DISPLAY
    mov al, 0xd ; carriage return
    int DISPLAY
    ret

hang:
    cli ; clear interrupt flag
    hlt ; halt the cpu
    jmp hang ; if for some reason we resume (likely due to an interrupt we cannot block), jump back to the hang sequence

disk_fail:
    print bad_disk
    jmp hang

[bits 32]
start_protected:

    ; setup stack
    mov esp, 0x9fc00 ; stack grows down, giving ~600kb of stack space
    mov ebp, esp ; set stack base at it's start

    ; every other segment register besides the cs register will use the data segment we defined in the GDT
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    call 0x7f00 ; call the location the kernel was loaded to

    jmp $

ok db "shrine_ok", 0 ; small confirmation message so we know the bootloader worked
bad_disk db "disk_bad", 0 ; small message to display in the case that we could not load data from the disk
times 510-($-$$) db 0 ; pad rest of 510 bytes with 0s
dw 0xaa55 ; bytes 511 and 512 need to be boot signature

; STRINGS/DATA FOR BOOTLOADER - 512 bytes - sector 2
load db "Loading ShrineOS...", 0
disk_okay db "Disk operations are functional.", 0
gdt_okay db "Loaded the GDT.", 0
protected_okay db "Switched from 16-bit real mode to 32-bit protected mode.", 0
finish db "ShrineOS is ready! We are going to have so much fun! :)", 0

; 114 bytes used so far

; global descriptor table
; it tends to be super weird and fragmented, but i'll try to comment as much as possible
gdt:
    ; gdt null descriptor
    ; describes a fake segment that is all null bytes
    ; seems to be a requirement for intel CPUs? likely to catch errors when setting the segment for far jumps
    dq 0x0 ; define 8 null bytes, as required

gdt_code_segment:
    ; gdt code segment descriptor
    ; size = f f f f f, base = 00 00 00 00
    ; flags 1 = 1 (present in memory) 00 (ring permission 0) 1 (code/data)
    ; flags 2 = 1 (multiply size by 4,000) 1 (32 bit) 0 (no 64 bit) 0 (avl bit, does nothing to hardware)
    ; type descriptor = 1 (code) 0 (protect from lower rings from executing segment) 1 (readable) 0 (lock bit / is being accessed)
    dw 0xffff ; the first 4 nibbles of the size of the segment: [f f f f] f
    dw 0x0 ; the lowest 2 bytes of the base address: 00 00 [00 00]
    db 0x0 ; the third lowest byte of the base address: 00 [00] 00 00
    db 10011010b ; first 4 bits is flags 1, last 4 bits is type descriptor
    db 11001111b ; first 4 bits is flags 2, last 4 bits is the last nibble of the size of the segment: f f f f [f]
    db 0x0 ; set highest byte of base address to 0: [00] 00 00 00
gdt_data_segment:
    ; gdt data segment descriptor, extremely similar to code segment with a change of type descriptor
    ; size = f f f f f, base = 00 00 00 00
    ; flags 1 = 1 (present in memory) 00 (ring permission 0) 1 (code/data)
    ; flags 2 = 1 (multiply size by 4,000) 1 (32 bit) 0 (no 64 bit) 0 (avl bit, does nothing to hardware)
    ; type descriptor = 0 (data) 0 (protect from lower rings from executing segment) 1 (readable) 0 (lock bit / is being accessed)
    dw 0xffff ; the first 4 nibbles of the size of the segment: [f f f f] f
    dw 0x0 ; the lowest 2 bytes of the base address: 00 00 [00 00]
    db 0x0 ; the third lowest byte of the base address: 00 [00] 00 00
    db 10010010b ; first 4 bits is flags 1, last 4 bits is type descriptor
    db 11001111b ; first 4 bits is flags 2, last 4 bits is the last nibble of the size of the segment: f f f f [f]
    db 0x0 ; set highest byte of base address to 0: [00] 00 00 00
gdt_descriptor:
    dw gdt_descriptor - gdt - 1 ; size of gdt - 1
    dd gdt ; address of gdt
CODE_SEGMENT equ gdt_code_segment - gdt ; define the code offset of gdt so we can tell the CPU which segment we refer to
DATA_SEGMENT equ gdt_data_segment - gdt ; define the data offset of gdt so we can tell the CPU which segment we refer to