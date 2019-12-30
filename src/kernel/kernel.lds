SECTIONS {
    . = 0x500;
    kernel_entry : { *(kernel_entry) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .rodata : { *(.rodata) }
}