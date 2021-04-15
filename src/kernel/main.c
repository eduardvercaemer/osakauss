#include <kernel/log.h>
#include <kernel/console.h>
#include <kernel/serial.h>
#include <kernel/GDT.h>
#include <kernel/IDT.h>
#include <kernel/ISR.h>
#include <kernel/IRQ.h>



const u32 magic = 0xdeadbeef;

void main() {
	gdt_init();
	idt_init();
	isr_init();
	irq_init();
	
	// we can select to log to serial, console, or both
	require_log(LOG_SERIAL);
	logf("   ...:::   asokauss v0.0.0  :::...\n\n");
	//logf("hello, %s, this is a hex number %x\n", "world", magic);
	//logf("and this is an escaped %%\n");
	//logf("and this is an integer %d\n", 6935);
	//logf("this is a negative integer %d\n", -421);
	
	if (require_console()) {
		logf("> console init successful\n");
	}
	
	// trigger breakpoint
	asm volatile("int $0x03");
 
	//loops forever
    for (;;);
}