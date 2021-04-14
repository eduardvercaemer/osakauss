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
    terminal_initialize();
	puts("hello, world");
    prints("hello, world"); // This prints a string
	
    //loops forever
    for (;;);
}