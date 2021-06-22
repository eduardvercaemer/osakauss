#include <x86.h>
#include <kernel/drivers/serial.h>
#include <kernel/ISR.h>
#include <kernel/IRQ.h>
#include <kernel/log.h>
#include <libs/stdlib.h>
#include <kernel/input.h>
#define PORT_COM1 0x03f8

static u8 require_satisfied;

static u8 serial_received(void);
static u8 is_transmit_empty(void);
static u8 serial_init(void);


struct SserialConsole{
	u32 offset;
	u32 barrier;
};

static struct SserialConsole SerialConsole = { 0,0 };

static u8
serial_received(void)
{
	return inb(PORT_COM1 + 5) & 1;
}

static u8
is_transmit_empty(void)
{
	return inb(PORT_COM1 + 5) & 0x20;
}

extern void handle_serial_in(struct regs *r)
{
	/* silence unused arg warning */
	r = r;

	char c = (char)serial_readb();
	switch ((int) c)
	{
	case 13:
		serial_writeb('\n');
		key_buffer_append('\n');
		break;
	case 127:
		serial_writeb('\b');
		key_buffer_append('\b');
		break;
	default:
		key_buffer_append(c);
		serial_writeb(c);
		break;
	}
}

// return 0 on success, 1 on error
static u8
serial_init(void)
{
	outb(PORT_COM1 + 1, 0x00);    // Disable all interrupts
	outb(PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outb(PORT_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outb(PORT_COM1 + 1, 0x00);    //                  (hi byte)
	outb(PORT_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(PORT_COM1 + 2, 0xc7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(PORT_COM1 + 4, 0x0b);    // IRQs enabled, RTS/DSR set
	outb(PORT_COM1 + 4, 0x1e);    // Set in loopback mode, test the serial chip
	outb(PORT_COM1 + 0, 0xae);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
	outb(PORT_COM1 + 1, 0x01);    // Enable all interrupts
	// Check if serial is faulty (i.e: not same byte as sent)
	if(inb(PORT_COM1 + 0) != 0xae) {
		return 1;
	}
	
	// If serial is not faulty set it in normal operation mode
	// (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
	outb(PORT_COM1 + 4, 0x0f);
	install_handler(36, handle_serial_in); // 36 = IRQ 4. interrupt for serial port
	return 0;
}

extern u8
require_serial(void)
{
	if (require_satisfied) return 1;
	
	// todo: is it fine to attempt to init more than once ?
	if (serial_init() == 1) return 0;
	
	return require_satisfied = 1;
}

extern int
serial_readb(void)
{
	while (serial_received() == 0) ;
	return inb(PORT_COM1);
}

extern void
serial_writeb(char val)
{
	if (val == '\b'){
		if (SerialConsole.offset <= SerialConsole.barrier);

		else{
			--SerialConsole.offset;
			while (is_transmit_empty() == 0);
			SerialConsole.offset++;
			outb(PORT_COM1, val);
			--SerialConsole.offset;
		}
	}
	else{
		while (is_transmit_empty() == 0);
		SerialConsole.offset++;
		outb(PORT_COM1, val);
	}
	
}

extern void
serial_printf(const char *fmt, ...)
{
	va_list args;
	
	void (*f)(char) = serial_writeb;
	
	va_start(args, fmt);
	formatv(f, fmt, args);
	va_end(args);
}
extern void
SerialSetBarrier(){
	SerialConsole.barrier = SerialConsole.offset;
	return;
}
extern void
SerialRemBarrier(){
	SerialConsole.barrier = 0;
	return;
}

