/*
Some of the code that belongs to isr is IDT.c
The location of IDT.c is kernel/IDT.c
 
*/
#include <kernel/ISR.h>
#include <kernel/console.h>

unsigned char *exception_messages[] =
{
    
    (unsigned char*)"Division by zero", // 0
    (unsigned char*)"Debug", // 1
    (unsigned char*)"Non maskable interrupt", // 2
    (unsigned char*)"Breakpoint", // 3
    (unsigned char*)"Into detected overflow", // 4
    (unsigned char*)"Out of bounds exception", //5
    (unsigned char*)"Invalid opcode", // 6
    (unsigned char*)"No coprocessor", // 7
    (unsigned char*)"Double fault", // 8
    (unsigned char*)"Coprocessor segment overrun", // 9
    (unsigned char*)"Bad TSS", // 10
    (unsigned char*)"Segment not present", // 11
    (unsigned char*)"Stack fault", // 12
    (unsigned char*)"General protection fault", // 13
    (unsigned char*)"Page fault", // 14
    (unsigned char*)"Unknown interrupt", // 15
    (unsigned char*)"Coprocessor fault", // 16
    (unsigned char*)"Alignment check", // 17
    (unsigned char*)"Machine check", // 18
    (unsigned char*)"Reserved", // 19
    (unsigned char*)"Reserved", // 20
    (unsigned char*)"Reserved", // 21
    (unsigned char*)"Reserved", // 22
    (unsigned char*)"Reserved", // 23
    (unsigned char*)"Reserved", // 24
    (unsigned char*)"Reserved", // 25
    (unsigned char*)"Reserved", // 26
    (unsigned char*)"Reserved", // 27
    (unsigned char*)"Reserved", // 28
    (unsigned char*)"Reserved", // 29
    (unsigned char*)"Reserved", // 30
    (unsigned char*)"Reserved", // 31
};


void fault_handler(struct regs *r)
{

    if (r->int_no < 32)
    {
        console_prints(exception_messages[r->int_no]);
        console_prints(" Exception. System Halted!\n");
        
        for (;;);
    }
    
}