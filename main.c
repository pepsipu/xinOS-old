#include <boot/multiboot.h>
#include <lib/mem.h>
#include <sys/idt.h>

struct multiboot_info mb_info;

void kmain(struct multiboot_info *mbinfo_ptr)
{
    // store the multiboot info in a global variable, but copy it since it'll be faster than derefering a pointer to the mb info
    memcpy(&mb_info, mbinfo_ptr, sizeof(struct multiboot_info));
    idt_init();
}