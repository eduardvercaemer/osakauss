#include <kernel/log.h>
#include <kernel/console.h>
#include <kernel/serial.h>
#include <kernel/GDT.h>
#include <kernel/IDT.h>

void main() {
	gdt_init();
	idt_init();
	isr_init();
	
	// we can select to log to serial, console, or both
	require_log(LOG_BOTH);
	logf("hello, %s, this is a hex number %x\n", "world", 0xdeadbeef);
	logf("and this is an escaped %%\n");
	logf("and this is an integer %d\n", 6935);
	logf("this is a negative integer %d\n", -421);
	
	// or we can simply use the methods they provide
	require_console(); // in this case this are no-ops, since log already required them
	require_serial();
	console_printf("with console_printf: %s\n", "hello");
	serial_printf("with serial_printf: %s\n", "hello");
	
	//asm volatile("int $0x03");
 
	//loops forever
    for (;;);
}