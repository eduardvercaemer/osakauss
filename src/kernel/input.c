#include <kernel/serial.h>
#include <kernel/input.h>
#include <types.h>
#include <kernel/log.h>
#include <stdlib.h>
#include <kernel/drivers/kb.h>

#define BUFFER_SIZE 2000 // size of tty screen 80*25


struct circlular_buffer{
    char buf[BUFFER_SIZE];
    u32 r;
    u32 w;
} key_buffer;

static struct {
	u8 serial;  // serial logging enabled
	u8 keyboard; // console logging enabled
} require_satisfied;

extern char 
read_key_buffer(bool blocking) {
    char c;
    if (blocking){
        while (key_buffer.w == key_buffer.r) {
            asm volatile("nop");
        }
    }
    c =  key_buffer.buf[ key_buffer.r];
    key_buffer.r = (key_buffer.r + 1) % BUFFER_SIZE;
    return c;
}

extern int 
input_read(char *buf,int size)
{
    if (require_satisfied.keyboard || require_satisfied.serial){
        int char_read = 0;
        while(char_read < size) {
            char c = read_key_buffer(true);
            if(c == 0) {
                return char_read;
            }
            *buf = c;
            buf++;
            char_read++; 
            
        }
        return char_read;
    }
    else{
        return 0;
    }
}


extern void 
key_buffer_append(char c) {
    if(c == 0) {
        return;
    }
    if(key_buffer.w == (key_buffer.r + BUFFER_SIZE - 1) % BUFFER_SIZE) {
        // circlular buffer. makes sure the buffer does not go out of bounds
        key_buffer.w = 0;
        key_buffer.r = 0;
    }
    key_buffer.buf[key_buffer.w] = c;
    key_buffer.w = (key_buffer.w + 1) % BUFFER_SIZE;
    return;
}

extern int 
require_input(enum input option){
    switch (option) {
		case INPUT_SERIAL:
			if (require_satisfied.serial) return 1;
			if (!require_serial()) return 0;
			require_satisfied.serial = 1;
			return 1;
		case INPUT_KEYBOARD:
            require_keyboard();
            require_satisfied.keyboard = 1;
            return 1;
		case INPUT_BOTH:
            if (!require_satisfied.serial) {
                if (!require_serial()) return 0;
                require_satisfied.serial = 1;
            }
            if (!require_satisfied.keyboard) {
                require_keyboard();
                require_satisfied.keyboard =  1;  
            }
            return 1;
		default:
			return 0;
    }
}









