#include "ports.h"

extern void
outb(u16 port, u8 val)
{
	__asm__ __volatile__ (
	"outb %0, %1\n\t"
	:
	: "a" (val), "Nd" (port)
	);
}

extern void
outw(u16 port, u16 val)
{
	__asm__ __volatile__ (
	"outw %0, %1\n\t"
	:
	: "a" (val), "Nd" (port)
	);
}

extern u8
inb(u16 port)
{
	u8 ret;
	
	__asm__ __volatile__ (
	"inb %1, %0\n\t"
	: "=a" (ret)
	: "Nd" (port)
	);
	
	return ret;
}

extern u16
inw(u16 port)
{
	u16 ret;
	
	__asm__ __volatile__ (
	"inw %1, %0\n\t"
	: "=a" (ret)
	: "Nd" (port)
	);
	
	return ret;
}
