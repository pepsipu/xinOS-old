SECTIONS {
    . = 0x500;
    kernel_entry : { *(kernel_entry) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}