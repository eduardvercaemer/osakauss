#pragma once
#include <types.h>
#include <stdarg.h>

/* libs/stdlib.c */
extern char *itoa(int value, char *str, int base);
extern u32 digit_count(int num);
extern usize strlen(const char* str);

extern int   memcmp(const void*, const void*, usize);
extern void *memcpy(void* __restrict, const void* __restrict, usize);
extern void *memmove(void*, const void*, usize);
extern void *memset(void*, u8, usize);

/* libs/format.c */
extern void format(void (*f)(char), const char *fmt, ...);
extern void formatv(void (*f)(char), const char *fmt, va_list args);