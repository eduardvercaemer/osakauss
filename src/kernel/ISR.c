/*
 * Some of the code that belongs to isr is IDT.c
 * The location of IDT.c is kernel/IDT.c
 */
#include <kernel/ISR.h>
#include <kernel/log.h>

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

/* used in assembly/kernel/IRS.S */
extern void
fault_handler(struct regs *r)
{
	// todo, add method to insert fault handlers
	if (r->int_no == 14) {
		return paging_page_fault(r);
	}
	
    if (r->int_no < 32) {
        tracef("fault: %s\n", exception_messages[r->int_no]);
        tracef("%s", "> halting \n");
        for (;;);
    }
}