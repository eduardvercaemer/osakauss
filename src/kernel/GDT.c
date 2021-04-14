#include <types.h>
#include <kernel/GDT.h>

static i32 GDT_SETUP = 0;

static struct gdt_entry gdt[3];
struct gdt_ptr gp; // not static: used in boot.S

/* assembly/kernel/GDT.S */
extern void gdt_flush();

static void
gdt_set_gate(i32 num, u64 base, u64 limit, u8 access, u8 gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

/* exports */

extern i32
gdt_init(void)
{
    if (GDT_SETUP == 1) return 1;
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (u32) &gdt;
    
    gdt_set_gate(0, 0, 0, 0, 0);
    
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush();
    return GDT_SETUP = 1;
}