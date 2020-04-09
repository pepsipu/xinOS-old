SECTIONS {
    . = 1M;
    kernel_entry : { *(kernel_entry) }
    .data : { *(.data) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
}