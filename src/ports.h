#ifndef OSAKAUSS_PORTS_H
#define OSAKAUSS_PORTS_H
#include "types.h"
// implementations on ports.c

/*
 * regular i386 port io methods
 */
extern void outb(u16 port, u8 val);
extern void outw(u16 port, u16 val);
extern u8   inb(u16 port);
extern u16  inw(u16 port);


#endif //OSAKAUSS_PORTS_H
