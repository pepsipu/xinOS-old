SECTIONS {
    . = 0x8200;
    kernel_entry : { *(kernel_entry) }
    .data : { *(.data) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
}