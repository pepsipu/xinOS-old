;***************************************************************************
; xinOS bootloader - bootloader.asm
; Sammy Hajhamid
; December 25, 2019
;
; Loads kernel and extra data (such success/error messages and the GDT)
; from the disk as well as sets up memory segments
;***************************************************************************

;[org 0x7c00] remove org directive since linker script will handle this

%define VESA_MODE 0x0111

%define DISK 0x13
%define DISPLAY 0x10

%macro print 1
    mov si, %1
    call print_si
%endmacro

; STAGE ONE - 512 bytes - sector 1
[bits 16]
stage_one_start:
    ; don't modify stack pointer and base pointer, on boot, bp = 0, and sp = to some value in between 0x7c00 (under bootloader) and 0x500 (above bios data)

    ; clear out all registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov sp, 0x7c00

    ; print confirmation that we have booted
    print ok

    ; read strings/data from disk to memory right above bootloader using CHS addressing
    mov bx, 0x7e00 ; data/strings location
    mov ah, 0x2 ; (bios) read a sector mode
    mov al, 0x15 ; read 15 sectors (7.5k)
    mov cx, 0x2 ; ch = cylinder 0, cl = sector 2
    mov dh, 0x0 ; head 0
    ; dl is set to correct drive number
    int DISK ; call bios to handle disk operation
    jc disk_fail ; if the carry flag is set (something went wrong) hang

    ; Print that we have successfully loaded the second stage
    print stage_two_loaded
    jmp stage_two_bootloader

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

;load_sector:
;    push ax
;    xor dx, dx
;
;    mov bx, [sectors_per_track] ; get sectors per track
;    div bx ; ax / bx
;    inc dx
;    push dx ; save sectors
;
;    xor dx, dx
;    mov bx, [heads]
;    div bx ; remainder (dx) is heads
;
;    pop cx ; sectors are on the stack
;    mov ch, al ; move cylinder into ch
;
;
;    mov ah, 0x2 ; (bios) load sector
;    mov al, 0x1 ; read 1 sector
;    mov dl, 0 ; floppy 0
;    mov bx, si
;    int DISK
;    jc disk_fail
;
;    pop ax
;    cmp di, 0
;    je pure_ret
;    dec di
;
;    inc ax
;    add si, 512
;
;    jmp load_sector

ok db "Bootloader stage one okay.", 0
stage_two_loaded db "Stage two loaded.", 0
bad_disk db "Unable to read from disk", 0 ; message to display in the case that we could not load data from the disk
times 510-($-$$) db 0 ; pad rest of 510 bytes with 0s
dw 0xaa55 ; bytes 511 and 512 need to be boot signature

; STAGE TWO - 7.5k bytes - sector 2-16
stage_two_bootloader:
    print load
    mov sp, 0x7c00
    call a20_test
    cmp ax, 1
    je a20_success

.int15_method:
    mov ax, 0x2403
    int 0x15
    jb .keyboard_controller_method
    cmp ah, 0
    jnz .keyboard_controller_method

    mov ax, 0x2402
    int 0x15
    jb .keyboard_controller_method
    cmp ah, 0
    jnz .keyboard_controller_method

    mov ax, 0x2401
    int 0x15
    jb .keyboard_controller_method
    cmp ah, 0
    jnz .keyboard_controller_method

    call a20_test
    cmp ax, 1
    je a20_success

.keyboard_controller_method:
    call keyboard_wait
    mov al,0xAD
    out 0x64,al

    call keyboard_wait
    mov al,0xD0
    out 0x64,al

    call keyboard_wait.other
    in al,0x60
    push eax

    call keyboard_wait
    mov al,0xD1
    out 0x64,al

    call keyboard_wait
    pop eax
    or al,2
    out 0x60,al

    call keyboard_wait
    mov al,0xAE
    out 0x64,al

    call keyboard_wait

    call a20_test
    cmp ax, 1
    je a20_success

.fast_a20_method:
    xor cx, cx
    in al, 0x92
    or al, 2
    and al, 0xFE
    out 0x92, al

    call a20_test
    cmp ax, 1
    je a20_success
    jmp a20_fail

a20_success:
    print a20_enabled

enter_protected_mode:
    cli ; disable interrupts because when we switch we do not want the BIOS handling our interrupts
    ; push real mode segments onto the stack
    push ds
    push es

    lgdt [gdt_descriptor] ; set addr of gdt descriptor so the cpu knows where our GDT is at
    print gdt_okay

    mov eax, cr0
    or al, 1 ; enable protected mode
    mov cr0, eax

    jmp protected_mode ; prevent crash

protected_mode:
    mov bx, 0x10
    ; set extra segments
    mov ds, bx
    mov es, bx

    and al, 0xfe
    mov cr0, eax

unreal_mode:
    pop es
    pop ds
    sti ; we enable interrupts again to be able to access BIOS functions
    ; now in unreal mode

.loop:
    hlt
    jmp .loop

    ; detect if LDA is available
    mov ah, 0x41 ; (bios) ensure lda
    mov bx, 0x55aa ; signature
    mov dl, 0x80 ; drive 0
    int DISK
    jc lda_bad ; err if carry is set

    mov ah, 0x42 ; (bios) read via LDA
    mov dl, 0x80 ; drive 0
    mov si, lda_packet
    int DISK
    jc disk_fail

    print load ; just a helpful message
    print disk_okay ; since we could load the strings, we know disk is fine

    ; switch to 32 bit protected mode
    ; first setup the global descriptor table which is defined in the sector we just loaded
    cli ; disable interrupts because when we switch we do not want the BIOS handling our interrupts
    lgdt [gdt_descriptor] ; set addr of gdt descriptor so the cpu knows where our GDT is at
    print gdt_okay

    ; vesa information is required in order to develop the graphical interface
    mov di, 0x8000 ; load VESA informtion at 0x8000
    mov ax, 0x4f00 ; (bios) query VESA information
    int DISPLAY

    mov si, word [0x8000 + 0xe] ; move offset of VESA modes into si
    call vesa_search_mode ; will hang if mode is not found
    ; if we reach here, the mode has been found

    mov di, 0x8000 ; write mode information here
    mov ax, 0x4f01 ; (bios) request mode information
    mov cx, VESA_MODE
    int DISPLAY

    mov ax, 0x4f02 ; (bios) set mode
    mov bx, VESA_MODE
    or bx, 0x4000 ; don't clear memory + use linear buffer
    int DISPLAY
    no_set:
    ; set 32 bit mode in control register
    mov esi, cr0 ; get contents of the control register so we can modify them
    or esi, 0x1 ; set esi's first bit (16 bit or 32 bit mode flag) to 1
    mov cr0, esi ; flag is now set, we go from 16 bit real mode to 32 bit protected mode
    jmp CODE_SEGMENT:start_protected ; make a far jump to flush the CPU pipeline so we don't keep executing 16 bit code

; AX is 0 if the A20 line is disabled, 1 otherwise
; https://wiki.osdev.org/A20_Line#Testing_the_A20_line
a20_test:
    xor ax, ax
    mov es, ax
    not ax
    mov fs, ax

    ; The addresses to be tested
    mov si, 0x7DFE
    mov di, 0x7E0E

    ; Check the values
    mov ax, word [es:si]
    cmp word [fs:di], ax
    jne .success

    ; Check that the values being exact wasn't a fluke
    rol ax, 4
    mov word [es:si], ax
    cmp word [fs:di], ax
    jne .success

    xor ax, ax ; ax = 0
    ret
.success:
    mov ax, 1
    ret

keyboard_wait:
    in al, 0x64
    test al, 2
    jnz keyboard_wait
    ret
.other:
    in al, 0x64
    test al, 1
    jz .other
    ret

a20_fail:
    print a20_not_enabled
    jmp hang

lda_bad:
    print no_lda
    jmp hang

vesa_fail:
    print graphics
    jmp no_set

vesa_search_mode:
    mov ax, [si] ; read a mode
    cmp ax, 0xffff ; end of list
    je vesa_fail ; hang if we reach end of the list
    cmp ax, VESA_MODE
    je .found_mode
    add si, 2
    jmp vesa_search_mode
.found_mode:
    ret

[bits 32]
start_protected:

    ; setup stack
    mov esp, 0x7c00 ; stack grows down, giving ~600kb of stack space (heap grows up from 0x7f00, though collision is unlikely)
    mov ebp, esp ; set stack base at it's start

    ; every other segment register besides the cs register will use the data segment we defined in the GDT
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    call 0x8200 ; call the location the kernel was loaded to

    jmp $

strings:
load db "Loading xinOS...", 0
a20_enabled db "A20 line enabled.", 0
a20_not_enabled db "Your PC does not support enabling the A20 line.", 0
disk_okay db "Disk operations are functional.", 0
no_lda db "Your PC does not support loading data from hard drives through LDA.", 0
gdt_okay db "Loaded the GDT.", 0
protected_okay db "Switching from 16-bit real mode to 32-bit protected mode.", 0
graphics db "Your video configurations are not supported.", 0

; heads db 0
; sectors_per_track db 0

; lda packet
align 4
lda_packet:
    db 0x10 ; size of LDA packet, which is 16 bytes
    db 0 ; required
    dw 127 ; sectors to transfer
    dw 0x8200 ; transfer buffer location
    dw 0x0 ; offset, which is 0
    dq 2 ; start reading including 3rd sector


; global descriptor table
; it tends to be super weird and fragmented, but i'll try to comment as much as possible
; there are 4 entries, the first 2 entries is code/data for 32 bit, the second 2 entries are for code/data in 16 bit

gdt:
    ; gdt null descriptor
    ; describes a fake segment that is all null bytes
    ; seems to be a requirement for intel CPUs? likely to catch errors when setting the segment for far jumps
    dq 0x0 ; define 8 null bytes, as required

    .code_segment:
    ; gdt code segment descriptor, for 32 bit protected mode
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
    .data_segment:
    ; gdt data segment descriptor, for 32 bit protected mode, extremely similar to code segment with a change of type descriptor
    ; size = f f f f f, base = 00 00 00 00
    ; flags 1 = 1 (present in memory) 00 (ring permission 0) 1 (code/data)
    ; flags 2 = 1 (multiply size by 4,000) 1 (32 bit) 0 (no 64 bit) 0 (avl bit, does nothing to hardware)
    ; type descriptor = 0 (data) 0 (protect from lower rings from executing segment) 1 (readable) 0 (lock bit / is being accessed)
    dw 0xffff ; the first 4 nibbles of the size of the segment: [f f f f] f
    dw 0x0 ; the lowest 2 bytes of the base address: 00 00 [00 00]
    db 0x0 ; the third lowest byte of the base address: 00 [00] 00 00
    db 10010010b ; first 4 bits is flags 1, last 4 bits is type descriptor
    db 11001111b ; first 4 bits is flags 2, last 4 bits is the last nibble of the size of the segment: f f f f [f]
    db 0x0 ; set highest byte of base address to 0: [00] 00 00 00.

gdt_descriptor:
    dw gdt_descriptor - gdt - 1 ; size of gdt - 1
    dd gdt ; address of gdt
CODE_SEGMENT equ gdt.code_segment - gdt ; define the code offset of gdt so we can tell the CPU which segment we refer to
DATA_SEGMENT equ gdt.data_segment - gdt ; define the data offset of gdt so we can tell the CPU which segment we refer to