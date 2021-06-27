#ifndef __INPUT_HEADER__
#define __INPUT_HEADER__
#include <types.h>
enum input {
	INPUT_SERIAL,
	INPUT_KEYBOARD,
    INPUT_BOTH
};
extern int require_input(enum input option);
extern int input_read(char *,int);
extern void key_buffer_append(char c);
extern char read_key_buffer(bool);
extern int input_readln(char *buf);
#endif