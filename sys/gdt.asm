global gdt_ptr

gdt_ptr:
    dw gdt.end - gdt - 1
    dq gdt

gdt:
    .null:
    dq 0x0
    .k_code_32:
    dd 0x0
    db 0x0
    db 10011010b
    db 00100000b
    db 0x0
    .k_data_32:
    dd 0x0
    db 0x0
    db 10010010b
    dw 0x0
    .u_code_32:
    dd 0x0
    db 0x0
    db 11111010b
    db 00100000b
    db 0x0
    .u_data_32:
    dd 0x0
    db 0x0
    db 11110010b
    dw 0x0
    .end: