#include "serial.h"

// todo: create a logging or printing api
void puts(const char *s)
{
	for (; *s; s++) {
		serial_writeb(*s);
	}
}

void main(){
	
	require_serial();
	puts("hello, world");
	
    //loops forever
    for (;;);
}