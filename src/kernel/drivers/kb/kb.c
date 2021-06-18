#include <kernel/ISR.h>
#include <kernel/IRQ.h>
#include <kernel/log.h>
#include <x86.h>
#include <kernel/input.h>
#include <kernel/drivers/console.h>

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b',	'\t','q', 'w', 'e', 'r',	
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',0,'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	
 '\'', '`',   0,'\\', 'z', 'x', 'c', 'v', 'b', 'n',	'm', ',', '.', '/',   0,'*',0,	' ',	0,	0,	
    0,   0,   0,   0,   0,   0,   0,   0,0,	0,	0,	0,	0,	0,	'-',0,	0,0,	'+',0,	0,0,0,	0,0,   0,   0,  0,	0,	0,	
};	

static char HandelScanCode(char scancode){
    char c = (char)kbdus[(u32)scancode];
    return c;
}

extern void keyboard_handler(struct regs *r)
{
    /* silent unused warning */
    r = r;

    unsigned char scancode;
    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {   
        char c = HandelScanCode(scancode);

        key_buffer_append(c);

        putch(c);
        
        
    }
}
		
void require_keyboard()
{
    install_handler(33, keyboard_handler);
}