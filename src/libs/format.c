#include <stdlib.h>

// the different formatters we have

static void format_string(void (*f)(char), const char *s);
static void format_i32_base(void (*f)(char), i32 n, u8 base);
static void format_i32(void (*f)(char), i32 n);
static void format_h32(void (*f)(char), u32 n);

static void
format_string(void (*f)(char), const char *s)
{
	for (; *s; s++) {
		f(*s);
	}
}

static void
format_i32_base(void (*f)(char), i32 n, u8 base)
{
	char buf[64];
	char *low, *ptr;
	const char *chars = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";
	
	// Check for supported base.
	if ( base < 2 || base > 36 ) {
		return;
	}
	
	low = ptr = (char *)buf;
	// Set '-' for negative decimals.
	if ( n < 0 && base == 10 ) {
		f('-');
	}
	
	// The actual conversion.
	do {
		// Modulo is negative for negative value. This trick makes abs() unnecessary.
		*ptr++ = chars[35 + n % base];
		n /= base;
	} while ( n );
	ptr--;
	
	// Invert the numbers.
	for (; low <= ptr; ptr-- ) {
		f(*ptr);
	}
}

static void
format_i32(void (*f)(char), i32 n)
{
	format_i32_base(f, n, 10);
}

static void
format_h32(void (*f)(char), u32 n)
{
	u32 tmp;
	
	f('0'); f('x');
	
	char noZeroes = 1;
	int i;
	for (i = 28; i > 0; i -= 4) {
		tmp = (n >> i) & 0xF;
		if (tmp == 0 && noZeroes != 0) {
			continue;
		}
		
		if (tmp >= 0xA) {
			noZeroes = 0;
			f(tmp - 0xA + 'a');
		}
		else {
			noZeroes = 0;
			f(tmp + '0');
		}
	}
	
	tmp = n & 0xF;
	if (tmp >= 0xA) {
		f(tmp - 0xA + 'a');
	}
	else {
		f(tmp + '0');
	}
}

/* exports */

extern void
format(void (*f)(char), const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	formatv(f, fmt, args);
	va_end(args);
}

extern void
formatv(void (*f)(char), const char *fmt, va_list args)
{
	for (; *fmt; fmt++) {
		char c = *fmt;
		switch (c) {
			case '%': // formatting
				switch (*(++fmt)) {
					case 's': { // string
						const char *s = va_arg (args, const char *);
						format_string (f, s);
						break;
					}
					case 'x': {// hex
						u32 n = va_arg (args, u32);
						format_h32 (f, n);
						break;
					}
					case 'd': {// int
						i32 n = va_arg (args, i32);
						format_i32 (f, n);
						break;
					}
					case '%': {// esacped %
						f ('%');
						break;
					}
				}
				break;
			default:
				f(c);
				break;
		}
	}
}