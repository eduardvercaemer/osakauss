#include <kernel/ISR.h>
#include <kernel/IRQ.h>
#include <kernel/log.h>
#include <x86.h>
#include <kernel/input.h>
#include <kernel/drivers/console.h>


struct Skeyboard{
    bool capslock;
}keyboard = {false};



unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	'9', '0', '-', '=', '\b',	'\t','q', 'w', 'e', 'r',	
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',0 /* Control key*/ ,'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	
 '\'', '`',   0,'\\', 'z', 'x', 'c', 'v', 'b', 'n',	'm', ',', '.', '/',   0,'*',0,	' ',	3/* caps lock */,	0,	
    0,   0,   0,   0,   0,   0,   0,   0,0,	0,	0,	0,	0,	0,	'-', 
    1, //  left arrow key
    0, 
    2, // right arrow key	
    '+',0,	0,0,0,	0,0,   0,   0,  0,	0,	0,	
};	

static char HandleScanCode(char scancode){
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
        char c = HandleScanCode(scancode);
        if (c == 2){
            MVCURSORC(1);
        }
        else if (c == 1){
            MVCURSORC(-1);
        }
        else if(c == 3){
            if (keyboard.capslock){
                keyboard.capslock = false;
            }
            else{
                keyboard.capslock =  true;
            }
        }
        else{
            
            if (keyboard.capslock && c >= 'a' &&  c <= 'z'){
                putch(c-32);
                key_buffer_append(c-32);
            }
            else{
                if (c == 0);
                else{
                    putch(c);
                    key_buffer_append(c);
                }
            }
            
            
            
        }        
        
    }
}
		
void require_keyboard()
{
    install_handler(33, keyboard_handler);
}