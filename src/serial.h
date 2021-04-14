#ifndef OSAKAUSS_SERIAL_H
#define OSAKAUSS_SERIAL_H
#include "types.h"
// implementations on serial.c

/*
 * ask for the serial module to load, you should call this before
 * using any other methods from here
 * returns: 1 on success, 0 on fail
 */
extern u8 require_serial(void);

/*
 * blocks until we can read a byte from serial
 */
extern char serial_readb(void);

/*
 * blocks until we can send a byte through serial
 */
extern void serial_writeb(char c);

#endif //OSAKAUSS_SERIAL_H
