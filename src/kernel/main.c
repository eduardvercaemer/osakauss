#include <kernel/serial.h>
#include <kernel/console.h>

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
	puts("hello, world");
	console_setcolor(VGA_COLOR_BLACK, VGA_COLOR_CYAN);
	console_prints("hello, world\n");
	console_printh(0xdeadbeef);
	
    //loops forever
    for (;;);
}