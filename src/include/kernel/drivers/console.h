#ifndef __CONSOLE_HEADER_
#define __CONSOLE_HEADER__
#include <types.h>

void putch(char c);

void printk(string s);

bool console_require();

extern void MVCURSORC(int);

extern void ConsoleSetBarrier();
extern void ConsoleRemBarrier();

#endif