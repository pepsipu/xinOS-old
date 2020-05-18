FLAGS equ 1 << 0 | 1 << 1 | 1 << 2
MAGIC equ 0x1badb002 ; signature
CHECKSUM equ -(MAGIC + FLAGS)
VIDMODE equ 0
WIDTH equ 1280
HEIGHT equ 720
DEPTH equ 24

global _start:function (_start.end - _start)
extern kmain
extern gdt_ptr

section .multiboot
    align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd VIDMODE
    dd WIDTH
    dd HEIGHT
    dd DEPTH
section .bss
    align 16
    resb 8192
    _esp: ; stack will grow down towards the end of the reserved bytes
section .text
    _start:
        mov esp, _esp
        push ebx
        cli
        lgdt [gdt_ptr]
        call kmain
        cli
        .hang:
        mov byte [0xb8000], ':'
        mov byte [0xb8002], '('
        hlt
        jmp .hang
        .end: