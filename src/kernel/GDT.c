#include <types.h>
#include <kernel/GDT.h>
#include <kernel/TSS.h>
#include <stdlib.h>



static i32 GDT_SETUP = 0;

static struct gdt_entry gdt[3];
struct gdt_ptr gp; // not static: used in boot.S

/* assembly/kernel/GDT.S */
extern void gdt_flush();

/* assembly/kernel/TSS.S */
extern void tss_flush();


static void write_tss(i32,u16,u32);


tss_entry_t tss_entry;


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

    gp.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gp.base = (u32) &gdt;
    
    
    gdt_set_gate(0, 0, 0, 0, 0);                
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); 
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); 
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); 
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); 



    write_tss(5, 0x10, 0x0);


    gdt_flush();
    
    tss_flush();
    return GDT_SETUP = 1;
}


static void 
write_tss(i32 num, u16 ss0, u32 esp0)
{
   u32 base = (u32) &tss_entry;
   u32 limit = base + sizeof(tss_entry);

   gdt_set_gate(num, base, limit, 0xE9, 0x00);

   memset(&tss_entry, 0, sizeof(tss_entry));

   tss_entry.ss0  = ss0;  
   tss_entry.esp0 = esp0; 

   tss_entry.cs   = 0x0b;
   tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
} 


extern void 
set_kernel_stack(u32 stack)
{
   tss_entry.esp0 = stack;
} 