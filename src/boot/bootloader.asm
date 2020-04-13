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

%define SIGNATURE 0x1234acef

%macro print 1
    mov si, %1
    call print_si
%endmacro

; STAGE ONE - 512 bytes - sector 1
[bits 16]
stage_one_start:
    ; clear out all registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov sp, 0x7c00

    cld ; clear direction flag

    ; print confirmation that we have booted
    print ok

    ; detect if LDA is available
    mov ah, 0x41 ; (bios) ensure lda
    mov bx, 0x55aa ; signature
    int DISK
    jc lda_bad ; err if carry is set
    print disk_okay

    ; read stage two of bootloader from disk to memory
    xor cx, cx
.retry_loop:
    push cx
    mov ah, 0x42 ; (bios) read a sector mode
    lea si, [lda_packet]
    ; dl is set to correct drive number
    int DISK ; call bios to handle disk operation
    jnc .check_signature ; if the carry flag is not set, check signature
    pop cx
.try_again:
    cmp cx, 3 ; check if this was our third try
    je disk_fail ; if it was, fail
    ; otherwise, try again
    inc cx
    jmp .retry_loop

.check_signature:
    pop cx
    cmp dword [stage_two_signature], SIGNATURE
    jne .try_again

.disk_success:
    ; Print that we have successfully loaded the second stage
    print stage_two_loaded
    jmp stage_two_bootloader

print_si:
    push eax
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
    pop eax
    ret

hang:
    cli ; clear interrupt flag
    hlt ; halt the cpu
    jmp hang ; if for some reason we resume (likely due to an interrupt we cannot block), jump back to the hang sequence

lda_bad:
    print no_lda
    jmp hang

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
no_lda db "Your PC does not support loading data from hard drives through LDA.", 0
bad_disk db "Unable to read from disk", 0 ; message to display in the case that we could not load data from the disk
disk_okay db "Disk operations are functional.", 0

; lda packet
align 4
lda_packet:
    db 0x10 ; size of LDA packet, which is 16 bytes
    db 0 ; required
.sectors:
    dw 15 ; sectors to transfer
.buffer_address:
    dw 0x7e00 ; transfer buffer location
    dw 0x0 ; offset, which is 0
.sector_start:
    dq 1 ; start reading from the 17th sector

times 510-($-$$) db 0 ; pad rest of 510 bytes with 0s
dw 0xaa55 ; bytes 511 and 512 need to be boot signature

; STAGE TWO - 7.5k bytes - sector 2-16
stage_two_signature:
    dd SIGNATURE
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

load_tar_header:
    ; load tar header containing kernel.bin
    xor cx, cx
    ; prepare lda to get tar file
    mov word [lda_packet.sectors], 1
    mov word [lda_packet.buffer_address], 0x9000
    mov dword [lda_packet.sector_start], 16
.retry_loop:
    push cx
    mov ah, 0x42 ; (bios) read a sector mode
    lea si, [lda_packet]
    ; dl is set to correct drive number
    int DISK ; call bios to handle disk operation
    jnc .check_magic ; if the carry flag is not set, check signature
    pop cx
.try_again:
    cmp cx, 3 ; check if this was our third try
    je disk_fail ; if it was, fail
    ; otherwise, try again
    inc cx
    jmp .retry_loop

.check_magic:
    pop cx
    cmp dword [0x9000 + 257], "usta"
    jne .try_again
    cmp word [0x9000 + 257 + 4], "r "
    jne .try_again

    ; 0x9000 now contains tar header
    mov ecx, 10
    mov esi, 0x907c
    call get_tar_field

    mov ecx, 0
    push ecx
load_kernel:
    mov word [lda_packet.buffer_address], 0x1000
    mov dword [lda_packet.sector_start], 17
    mov edi, 0x100000
    ;mov edi, 0x8500

    ; bytes to sectors
    mov ecx, ebx
    add ecx, 511 ; align to 512 bytes
    shr ecx, 9 ; divide by 512 bytes to convert to sectors
load_kernel_block:
    push ecx
    push edi
    ;xor ecx, ecx
;.lda_retry:
    mov ah, 0x42 ; (bios) read via LDA
    lea si, [lda_packet]
    int DISK
    jc disk_fail
    
    ;cmp ecx, 2
    ;jmp disk_fail
    ;inc ecx
    ;jmp .lda_retry

.continue:
    pop edi
    mov ecx, 0
    movzx esi, word [lda_packet.buffer_address]
.move_loop:
    ; Move buffer from esi to edi, ecx times
    mov eax, [esi + ecx * 4]
    mov [es:edi + ecx * 4], eax
    inc ecx
    cmp ecx, 512 / 4
    jle .move_loop
    add edi, 512

    ; Prepare for next loop
    mov eax, [lda_packet.sector_start]
    add eax, 1
    mov [lda_packet.sector_start], eax

    pop ecx
    sub ecx, 1
    jnz load_kernel_block

    pop ecx
    sub ecx, 1
    push ecx
    cmp ecx, 0
    je load_kernel

    ; vesa information is required in order to develop the graphical interface
    mov di, 0x9000 ; load VESA informtion at 0x9000
    mov ax, 0x4f00 ; (bios) query VESA information
    int DISPLAY

    mov si, word [0x9000 + 0xe] ; move offset of VESA modes into si
    call vesa_search_mode ; will hang if mode is not found
    ; if we reach here, the mode has been found

    mov di, 0x9000 ; write mode information here
    mov ax, 0x4f01 ; (bios) request mode information
    mov cx, VESA_MODE
    int DISPLAY

    mov ax, 0x4f02 ; (bios) set mode
    mov bx, VESA_MODE
    or bx, 0x4000 ; don't clear memory + use linear buffer
    int DISPLAY

    ; Finally switch to 32 bit protected mode
    cli ; disable interrupts because when we switch we do not want the BIOS handling our interrupts
    lgdt [gdt_descriptor] ; set addr of gdt descriptor so the cpu knows where our GDT is at

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEGMENT:start_protected ; make a far jump to flush the CPU pipeline
.loop:
    hlt
    jmp .loop

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

; Input - ECX = amount of iterations, ESI = field location
; Output - EBX = size
get_tar_field:
    push eax
    push edx
    mov eax, 1 ; count
    xor ebx, ebx ; size
    ; ecx = loop iterator
    xor edx, edx
.loop:
    push eax
    mov dl, byte [esi + ecx]
    sub edx, 0x30
    mul edx
    add ebx, eax

    pop eax
    mul dword [tar_octal_mul]
    dec ecx
    jnz .loop
    pop edx
    pop eax
    ret

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

vesa_fail:
    print graphics
    jmp hang

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

    call 0x100000 ; call the location the kernel was loaded to

    jmp $

strings:
load db "Loading xinOS...", 0
a20_enabled db "A20 line enabled.", 0
a20_not_enabled db "Your PC does not support enabling the A20 line.", 0
gdt_okay db "Loaded the GDT.", 0
protected_okay db "Switching from 16-bit real mode to 32-bit protected mode.", 0
graphics db "Your video configurations are not supported.", 0
consts:
tar_octal_mul dd 8

; heads db 0
; sectors_per_track db 0

; global descriptor table
; it tends to be super weird and fragmented, but i'll try to comment as much as possible
; there are 2 code/data entries for 32 bit

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