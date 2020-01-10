SECTIONS {
    . = 0x100000;
    kernel_entry : { *(kernel_entry) }
    .data : { *(.data) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
}