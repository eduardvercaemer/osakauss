#ifndef __SERIAL_HEADER__
#define __SERIAL_HEADER__
#include <types.h>
#include <libs/stdlib.h>

/* kernel/serial.c */

/*
 * ask for the serial module to load, you should call this before
 * using any other methods from here
 * returns: 1 on success, 0 on fail
 */
extern u8 require_serial(void);

/*
 * blocks until we can read a byte from serial
 */
extern int serial_readb(void);

/*
 * blocks until we can send a byte through serial
 */
extern void serial_writeb(char c);

/*
 * write a format string to serial
 */
extern void serial_printf(const char *fmt, ...);
extern void SerialSetBarrier();
extern void SerialRemBarrier();
#endif