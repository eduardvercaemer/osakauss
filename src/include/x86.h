#pragma once

static inline unsigned char inb(unsigned short port)
{
  unsigned char data;

  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void insl(int port, void *addr, int cnt)
{
  asm volatile("cld; rep insl" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

static inline void outb(unsigned short port, unsigned char data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void outw(unsigned short port, unsigned short data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void outsl(int port, const void *addr, int cnt)
{
  asm volatile("cld; rep outsl" :
               "=S" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "cc");
}
