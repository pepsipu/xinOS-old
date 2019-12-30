SECTIONS {
    . = 0x7f00;
    kernel_entry : { *(kernel_entry) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .rodata : { *(.rodata) }
}