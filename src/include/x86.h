#pragma once
#include <types.h>

static inline u8 inb(u16 port)
{
  u8 data;

  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void insl(u16 port, void *addr, usize cnt)
{
  asm volatile("cld; rep insl" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

static inline void outb(u16 port, u8 data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void outw(u16 port, u16 data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void outsl(u16 port, const void *addr, usize cnt)
{
  asm volatile("cld; rep outsl" :
               "=S" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "cc");
}

static inline void hang(void)
{
	for (;;) ;
}