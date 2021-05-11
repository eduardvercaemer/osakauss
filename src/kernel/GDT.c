#include <kernel/GDT.h>

__attribute__((aligned(0x1000)))
GDT DefaultGDT = {
    {0, 0, 0, 0x00, 0x00, 0}, // null
    {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
    {0, 0, 0, 0x00, 0x00, 0}, // user null
    {0, 0, 0, 0x9a, 0xa0, 0}, // user code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // user data segment
};



/*
//some code that may work if this one does not

struct gdt_descriptor {
    u32 _0;
    u8 _1, access, granularity, _2;
} __attribute__((packed));

struct gdt_pointer {
    u16 size; u64 addr;
} __attribute__((packed));

const u8 access_flags = 0b10010010;  // Present, ring 0 only, readable cs and writable data
const u8 gdt_is_code_segment = 1 << 3, gdt_longmode_cs = 1 << 5;

static struct gdt_descriptor gdt[] = {
    {},
    {.access = access_flags | gdt_is_code_segment, .granularity = gdt_longmode_cs},  // kern cs
    {.access = access_flags, .granularity = 0} // kern ds
};

void gdt_load() {
    struct gdt_pointer gdtr = {.size = sizeof(gdt) - 1, .addr = (u64)&gdt};
    asm volatile("lgdt %0\n\t" : : "m"(gdtr));
    asm volatile("push $0x08\npushq $1f\nlretq\n1:\n" : :);
    asm volatile("mov %0, %%ds\nmov %0, %%es\nmov %0, %%gs\nmov %0, %%fs\nmov %0, %%ss\n" : : "a"((u16)0x10));
}

*/