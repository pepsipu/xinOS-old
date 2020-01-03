SECTIONS {
    . = 0x500;
    kernel_entry : { *(kernel_entry) }
    .data : { *(.data) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
}