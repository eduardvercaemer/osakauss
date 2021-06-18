#include <stddef.h>
#include <types.h>
#include <libs/stdlib.h>
#include <x86.h>

static const u32 VGA_WIDTH = 80;
static const u32 VGA_HEIGHT = 25;
static u32 terminal_row;
static u32 terminal_column;
static u8 terminal_color;
static u16 * terminal_buffer;


static void  init_cursor(void);

static void enable_cursor(void);


static void update_cursor(int x, int y);


static enum colours {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};


static inline uint8_t vga_entry_color(enum colours fg, enum colours bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

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

bool console_require() 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	init_cursor();
	for (u32 y = 0; y < VGA_HEIGHT; y++) {
		for (u32 x = 0; x < VGA_WIDTH; x++) {
			const u32 index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}


static void putchat(char c, u8 color, u32 x, u32 y){
    const u32 index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void putch(char c) 
{
	if (c == '\n'){
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
		putchat(c, terminal_color, terminal_column, terminal_row);
		++terminal_column;
	}
	scroll();
	update_cursor(terminal_column,terminal_row);
}
 
static void write(string s, u32 size) 
{
	for (u32 i = 0; i < size; i++)
		putch(s[i]);
}

void printk(string s) 
{
	write(s, strlen(s));
}


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


static void
update_cursor(int x, int y)
{	

	u16 pos = terminal_row * VGA_WIDTH + terminal_column;	
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}