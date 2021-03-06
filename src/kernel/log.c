#include <kernel/log.h>
#include <kernel/serial.h>
#include <kernel/console.h>
#include <lambda.h>
#include <stdlib.h>

static struct {
	u8 serial;  // serial logging enabled
	u8 console; // console logging enabled
} require_satisfied;

/* exports */

bool trace = false;

extern u8
require_log(enum logging_output output)
{
	switch (output) {
		case LOG_SERIAL:
			if (require_satisfied.serial) return 1;
			if (!require_serial()) return 0;
			require_satisfied.serial = 1;
			return 1;
		case LOG_CONSOLE:
			if (require_satisfied.console) return 1;
			if (!require_console()) return 0;
			require_satisfied.console = 1;
			return 1;
		case LOG_BOTH:
			return require_log(LOG_SERIAL) && require_log(LOG_CONSOLE);
		default:
			return 0;
	}
}

extern void
logf(const char *fmt, ...)
{
	va_list args;
	
	va_start(args, fmt);
	void (*f)(char) = lambda(void, (char c) {
		if (require_satisfied.console)
			console_putch(c);
		if (require_satisfied.serial)
			serial_writeb(c);
	});
	formatv(f, fmt, args);
	va_end(args);
}