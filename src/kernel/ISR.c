/*
 * Some of the code that belongs to isr is IDT.c
 * The location of IDT.c is kernel/IDT.c
 */
#include <kernel/ISR.h>
#include <kernel/IDT.h>
#include <kernel/log.h>
#include <stdlib.h>
#include <x86.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

static void (*
interrupt_handlers[256])(regs_t *);

static const char *exception_messages[] = {
    "Division by zero", // 0
    "Debug", // 1
    "Non maskable interrupt", // 2
    "Breakpoint", // 3
    "Into detected overflow", // 4
    "Out of bounds exception", //5
    "Invalid opcode", // 6
    "No coprocessor", // 7
    "Double fault", // 8
    "Coprocessor segment overrun", // 9
    "Bad TSS", // 10
    "Segment not present", // 11
    "Stack fault", // 12
    "General protection fault", // 13
    "Page fault", // 14
    "Unknown interrupt", // 15
    "Coprocessor fault", // 16
    "Alignment check", // 17
    "Machine check", // 18
    "Reserved", // 19
    "Reserved", // 20
    "Reserved", // 21
    "Reserved", // 22
    "Reserved", // 23
    "Reserved", // 24
    "Reserved", // 25
    "Reserved", // 26
    "Reserved", // 27
    "Reserved", // 28
    "Reserved", // 29
    "Reserved", // 30
    "Reserved", // 31
};

/* static methods */

static void
irq_remap(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

/* exports */

/* used in assembly/kernel/IRS.S */
extern void
fault_handler(regs_t *r)
{
    static char buf[33];

    u8 int_no = r->int_no & 0xFF;

    if (interrupt_handlers[int_no] != 0) {
        void (*handler)(regs_t *r);
        handler = interrupt_handlers[int_no];
        handler(r);
    } else {
        if (r->int_no < 32) {
            logf("fault: %s\nhalting system\n", exception_messages[r->int_no]);
            for (;;);
        } else {
            logf("Unknown interrupt\nfault number: %s\nhalting system\n", itoa(r->int_no,buf,10));
            for (;;);
        }
    }
}

extern void
install_handler(int irq, void (*handler)(regs_t *r))
{
    interrupt_handlers[irq] = handler;
}

extern void
uninstall_handler(int irq)
{
    interrupt_handlers[irq] = 0;
}

extern void
irq_init()
{
    irq_remap();

    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

extern void
irq_handler(regs_t *r)
{
    void (*handler)(regs_t *r);
    handler = interrupt_handlers[r->int_no];
    if (handler) {
        handler(r);
    }

    if (r->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}
