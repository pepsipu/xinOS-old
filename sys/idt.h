#include <stdint.h>

void register_isr(void *isr, uint8_t index);
void idt_init();