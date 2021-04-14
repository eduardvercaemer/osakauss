#include <kernel/serial.h>
#include <kernel/console.h>
#include <kernel/GDT.h>
#include <kernel/IDT.h>

// todo: create a logging or printing api
void puts(const char *s)
{
	for (; *s; s++) {
		serial_writeb(*s);
	}
}

void main(){
	
	require_serial();
    require_console();


	gdt_init();

	idt_init();

	isr_init();
	
	puts("hello, world");
	console_setcolor(VGA_COLOR_BLACK, VGA_COLOR_CYAN);
	console_prints("hello, world\n");
	console_printh(0xdeadbeef);
	
	
	//asm volatile("int $0x03");
    //loops forever
    for (;;);
}