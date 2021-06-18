#include <kernel/IRQ.h>
#include <x86.h>
#include <libs/stdlib.h>
#include <kernel/log.h>


#define PIC1		0x20		
#define PIC2		0xA0	
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

static void (*
interrupt_handlers[256])(struct regs *);



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



extern void
install_handler(int irq, void (*handler)(struct  regs *r))
{
    interrupt_handlers[irq] = handler;
}

extern void
uninstall_handler(int irq)
{
    interrupt_handlers[irq] = 0;
}


extern void
fault_handler(struct regs *r)
{   char buf[33];
    if (interrupt_handlers[r->interrupt] != 0) {
        void (*handler)(struct regs *r);
        handler = interrupt_handlers[r->interrupt];
        handler(r);
    } else {
        if (r->interrupt < 32) {
            logf("fault: %s\nhalting system\n", exception_messages[r->interrupt]);
            for (;;);
        } else {
            logf("Unknown interrupt\nfault number: %s\nhalting system\n", itoa(r->interrupt,buf,10));
            for (;;);
        }
    }
}

extern void
irq_handler(struct regs *r)
{
    void (*handler)(struct regs *r);
    handler = interrupt_handlers[r->interrupt];
    if (handler) {
        handler(r);
    }

    if (r->interrupt >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

void IRQSetMask(u8 IRQline){
    u16 port;
    u8 value;
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
void IRQClearMask(u8 IRQline) {
    u16 port;
    u8 value;
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}
