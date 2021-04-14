#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <x86.h>

/* Hardware text mode color constants. */
enum vga_color {
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
// Scrolls the text on the screen up by one line.

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
    init_cursor();
}

//This scrolls the screen
static void scroll()
{
   uint16_t blank = vga_entry(' ',terminal_color);
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

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void putch(char c) 
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
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		putch(data[i]);
}
 
void prints(const char* data) 
{
	terminal_write(data, strlen(data));
    
    
}



void printh(uint32_t n)
{
    uint32_t tmp;

    prints("0x");

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
    
        if (tmp >= 0xA)
        {
            noZeroes = 0;
            putch (tmp-0xA+'a' );
        }
        else
        {
            noZeroes = 0;
            putch( tmp+'0' );
        }
    }
  
    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        putch (tmp-0xA+'a');
    }
    else
    {
        putch (tmp+'0');
    }

}



void printi(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num,10);
  prints(str_num);
}

void printb(uint32_t num)
{
  char bin_arr[32];
  uint32_t index = 31;
  uint32_t i;
  while (num > 0){
    if(num & 1){
      bin_arr[index] = '1';
    }else{
      bin_arr[index] = '0';
    }
    index--;
    num >>= 1;
  }

  for(i = 0; i < 32; ++i){
   if(i <= index)
      putch('0');
   else
     putch(bin_arr[i]);
  }
}






int cursor_x = 0;
int cursor_y = 0;

void init_cursor(){
	enable_cursor();
	update_cursor(0,0);
}

void enable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}


void update_cursor(int x, int y)
{
    cursor_x = x;
    cursor_y = y;

	uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}




uint16_t get_cursor_position(void)
{
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}