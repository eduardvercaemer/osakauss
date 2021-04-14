#pragma once

#include <stdint.h>

void terminal_setcolor(uint8_t color);
void prints(const char* data);
void printh(uint32_t n);
void printi(int num);
void printb(uint32_t num);

void terminal_initialize(void);