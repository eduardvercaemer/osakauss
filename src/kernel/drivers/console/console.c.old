#include <stddef.h>
#include <types.h>
#include <libs/stdlib.h>
#include <x86.h>
#include <kernel/drivers/console.h>


static void  init_cursor(void);

static void enable_cursor(void);


static void update_cursor();


struct Sconsole{
	u32 offset;
	u32 barrier;
	u16 * buffer;
	u8 color;
	u32 column;
	u32 row;
	u32 width;
	u32 height;
	u32 crws; // means: current read and write status
};


static struct Sconsole console = {
	.barrier = 0,
	.offset = 0,
};  



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
	u16 blank = vga_entry(' ',console.color);
	if(console.row >= 25)
	{
		int i;
		for (i = 0*80; i < 24*80; i++)
		{
			console.buffer[i] = console.buffer[i+80];
		}
		for (i = 24*80; i < 25*80; i++)
		{
			console.buffer[i] = blank;
		}
		console.row = 24;
		update_cursor(console.column,console.row);
	}
	
}

bool console_require() 
{
	console.height = 25;
	console.width = 80;
	console.row = 0;
	console.column = 0;
	console.color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	console.buffer = (uint16_t*) 0xB8000;
	init_cursor();
	for (u32 y = 0; y < console.height; y++) {
		for (u32 x = 0; x < console.width; x++) {
			const u32 index = y * console.width + x;
			console.buffer[index] = vga_entry(' ', console.color);
		}
	}
}


static void putchat(char c, u8 color, u32 x, u32 y){ // meaning: put char at ( this puts a char a postion entered)
    const u32 index = y * console.width + x;
	console.buffer[index] = vga_entry(c, color);
}

void putch(char c) 
{
	if (c == '\n'){
		++console.row;
		console.column=0;
	}
	
	else if (c == '\b'){
		if (console.column == 0){
		
		}
		else{
				if (console.barrier == 0){
					--console.column;
					--console.offset;
					putch(' ');
					--console.column;
					--console.offset;
				}
				else if (console.barrier >= console.offset);
				else{
					--console.column;
					--console.offset;
					putch(' ');
					--console.column;
					--console.offset;
					
				}
			
		}
		
	}
	else if (c == '\t'){
		for (int x=0; x!=4; x++){
			putch(' ');
		}
	}
	else if (c == '\r'){
		console.column=0;
	}
	
	else{
		putchat(c, console.color, console.column, console.row);
		++console.column;
		++console.offset;
	}
	scroll();
	update_cursor(console.column,console.row);
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
	update_cursor();
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
update_cursor()
{	

	u16 pos = console.row * console.width + console.column;	
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}

extern void 
MVCURSORC(int i){ // what it means: 'move cursor column'.
	console.column = console.column + i;
	update_cursor();
}
extern void
ConsoleSetBarrier(){ //  set barrier
	console.barrier = console.offset;
	return;
}
extern void
ConsoleRemBarrier(){ // remove barrier
	console.barrier = 0;
	return;
}