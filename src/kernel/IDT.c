#include <types.h>
#include <kernel/IDT.h>
#include <kernel/console.h>
#include <stdlib.h>

struct idt_entry {
    u16 base_lo;
    u16 sel;
    u8 always0;
    u8 flags;
    u16 base_hi;
} __attribute__((packed));
struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));
struct idt_entry idt[256];
struct idt_ptr idtp;

/* defined in assembly/kernel/IDT.S */
extern void idt_load();

/* defined in assembly/kernel/ISR.S */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr128();
extern void idt_flush(u32);

/* exports */

extern void
isr_init(void)
{
   idt_set_gate( 0, (u32)isr0 , 0x08, 0x8E);
   idt_set_gate( 1, (u32)isr1 , 0x08, 0x8E);
   idt_set_gate( 2, (u32)isr2 , 0x08, 0x8E);
   idt_set_gate( 3, (u32)isr3 , 0x08, 0x8E);
   idt_set_gate( 4, (u32)isr4 , 0x08, 0x8E);
   idt_set_gate( 5, (u32)isr5 , 0x08, 0x8E);
   idt_set_gate( 6, (u32)isr6 , 0x08, 0x8E);
   idt_set_gate( 7, (u32)isr7 , 0x08, 0x8E);
   idt_set_gate( 8, (u32)isr8 , 0x08, 0x8E);
   idt_set_gate( 9, (u32)isr9 , 0x08, 0x8E);
   idt_set_gate( 10, (u32)isr10 , 0x08, 0x8E);
   idt_set_gate( 11, (u32)isr11 , 0x08, 0x8E);
   idt_set_gate( 12, (u32)isr12 , 0x08, 0x8E);
   idt_set_gate( 13, (u32)isr13 , 0x08, 0x8E);
   idt_set_gate( 14, (u32)isr14 , 0x08, 0x8E);
   idt_set_gate( 15, (u32)isr15 , 0x08, 0x8E);
   idt_set_gate( 16, (u32)isr16 , 0x08, 0x8E);
   idt_set_gate( 16, (u32)isr16 , 0x08, 0x8E);
   idt_set_gate( 16, (u32)isr16 , 0x08, 0x8E);
   idt_set_gate( 17, (u32)isr17 , 0x08, 0x8E);
   idt_set_gate( 18, (u32)isr18 , 0x08, 0x8E);
   idt_set_gate( 19, (u32)isr19 , 0x08, 0x8E);
   idt_set_gate( 20, (u32)isr20 , 0x08, 0x8E);
   idt_set_gate( 21, (u32)isr21 , 0x08, 0x8E);
   idt_set_gate( 22, (u32)isr22 , 0x08, 0x8E);
   idt_set_gate( 23, (u32)isr23 , 0x08, 0x8E);
   idt_set_gate( 24, (u32)isr24 , 0x08, 0x8E);
   idt_set_gate( 25, (u32)isr25 , 0x08, 0x8E);
   idt_set_gate( 26, (u32)isr26 , 0x08, 0x8E);
   idt_set_gate( 27, (u32)isr27 , 0x08, 0x8E);
   idt_set_gate( 28, (u32)isr28 , 0x08, 0x8E);
   idt_set_gate( 29, (u32)isr29 , 0x08, 0x8E);
   idt_set_gate( 30, (u32)isr30 , 0x08, 0x8E);
   idt_set_gate( 31, (u32)isr31 , 0x08, 0x8E);
   idt_set_gate( 128, (u32)isr128 , 0x08, 0x8E);

   idt_flush((u32)&idtp);
}

/* exports */

extern void
idt_set_gate(u8 num, unsigned long base, u16 sel, u8 flags)
{
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

extern void
idt_init(void)
{
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (u32)&idt;
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    idt_load();
}