#include <kernel/log.h>
#include <kernel/console.h>
#include <kernel/serial.h>
#include <kernel/paging.h>
#include <kernel/GDT.h>
#include <kernel/IDT.h>
#include <kernel/ISR.h>
#include <kernel/IRQ.h>

const u32 magic = 0xdeadbeef;

void main() {
	require_log(LOG_SERIAL);
	logf("   ...:::   asokauss v0.0.0  :::...\n\n");
	
	// todo, move all these into descriptors_init or similar
	gdt_init();
	idt_init();
	isr_init();
	irq_init();
	
	paging_init();
	
	logf("this is a message after setting up paging !\n\n");
	
	// test kmalloc
	u32 *p = kmalloc(16);
	
	// trigger page fault
	u32 *ptr = (u32*)0xa0000000;
	u32 tmp;
	logf("reading from [%p]\n", 0xa0000000);
	tmp = *ptr;
	logf("value read: %d\n", tmp);
	logf("writing to [%p]\n", 0xa0000000);
	*ptr = 0xdeadbeef;
	
	// trigger breakpoint
	asm volatile("int $3");
 
	//loops forever
    for (;;);
}