#include <kernel/console.h>
#include <stdlib.h>
#include <lambda.h>
#include <x86.h>

/* static members */

static u8 require_satisfied = 0;

static const usize VGA_WIDTH = 80;
static const usize VGA_HEIGHT = 25;

static usize terminal_row;
static usize terminal_column;
static u8 terminal_color;
static u16* terminal_buffer;

static int cursor_x = 0;
static int cursor_y = 0;

/* static methods */

static void update_cursor(int x, int y);
static void enable_cursor(void);
static void scroll(void);

static inline u8
vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}
 
static inline u16
vga_entry(unsigned char uc, u8 color)
{
	return (u16) uc | (u16) color << 8;
}

//This scrolls the screen
static void
scroll(void)
{
	u16 blank = vga_entry(' ',terminal_color);
	if(terminal_row >= 25)
	{
		int i;
		for (i = 0*80; i < 24*80; i++)
		{
			terminal_buffer[i] = terminal_buffer[i+80];
		}
		for (i = 24*80; i < 25*80; i++)
		{
			terminal_buffer[i] = blank;
		}
		terminal_row = 24;
		update_cursor(terminal_column,terminal_row);
	}
	
}

static void
terminal_putentryat(char c, u8 color, usize x, usize y)
{
	const usize index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

// todo, handle escape sequences to allow color changing through writing etc
static void
putch(char c)
{   if (c == '\n'){
		++terminal_row;
		terminal_column=0;
	}
	
	else if (c == '\b'){
		if (terminal_column == 0){
		
		}
		else{
			--terminal_column;
			putch(' ');
			--terminal_column;
		}
		
	}
	else if (c == '\t'){
		for (int x=0; x!=4; x++){
			putch(' ');
		}
	}
	else if (c == '\r'){
		terminal_column=0;
	}
	
	else{
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		++terminal_column;
	}
	scroll();
	update_cursor(terminal_column,terminal_row);
}

/* UNUSED
static void
terminal_write(const char* data, usize size)
{
	for (usize i = 0; i < size; i++)
		putch(data[i]);
}
*/

static void
init_cursor(void){
	enable_cursor();
	update_cursor(0,0);
}

static void
enable_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

/* UNUSED
static void
disable_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}
*/

static void
update_cursor(int x, int y)
{
	cursor_x = x;
	cursor_y = y;
	
	u16 pos = cursor_y * VGA_WIDTH + cursor_x;
	
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}

/* UNUSED
static u16
get_cursor_position(void)
{
	u16 pos = 0;
	outb(0x3D4, 0x0F);
	pos |= inb(0x3D5);
	outb(0x3D4, 0x0E);
	pos |= ((u16)inb(0x3D5)) << 8;
	return pos;
}
*/

/* public exports */

extern u8
require_console(void)
{
	if (require_satisfied) return 1;
	
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (u16*) 0xB8000;
	for (usize y = 0; y < VGA_HEIGHT; y++) {
		for (usize x = 0; x < VGA_WIDTH; x++) {
			const usize index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	init_cursor();

	return require_satisfied = 1;
}

extern void
console_setcolor(u8 fg, u8 bg)
{
	terminal_color = vga_entry_color(fg, bg);
}

extern void
console_printf(const char *fmt, ...)
{
	va_list args;
	
	void (*f)(char) = console_putch;

	va_start(args, fmt);
	formatv(f, fmt, args);
	va_end(args);
}

extern void
console_puts(const char *s)
{
	for (; *s; s++) {
		console_putch(*s);
	}
}

extern void
console_putch(char c)
{
	putch(c);
}
