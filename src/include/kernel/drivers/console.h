#pragma once
#include <types.h>

void putch(char c);

void printk(string s);

bool console_require();

extern void MVCURSORC(int);

extern void SetBarrier();