#pragma once
#include <types.h>
#include <kernel/log.h>
#include <stdlib.h>


#define PANIC(msg) panic(msg, __FILE__, __LINE__);


static inline u8 
inb(u16 port)
{
  u8 data;

  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
  return data;
}

static inline void 
insl(u16 port, void *addr, usize cnt)
{
  asm volatile("cld; rep insl" :
               "=D" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "memory", "cc");
}

static inline void 
outb(u16 port, u8 data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void 
outw(u16 port, u16 data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void 
outsl(u16 port, const void *addr, usize cnt)
{
  asm volatile("cld; rep outsl" :
               "=S" (addr), "=c" (cnt) :
               "d" (port), "0" (addr), "1" (cnt) :
               "cc");
}

static inline void 
sti()
{
  asm volatile("sti");
}
static inline void 
cli()
{
  asm volatile("cli");
}

static inline void 
enable_interrupts()
{
  sti();
}

static inline void 
hang(void)
{
	for (;;) ;
}

static inline void 
panic(const char *message, const char *file, u32 line)
{
    cli();
    
    static char b[33];

    logf("PANIC( %s ) at %s:%s\n",message,file,itoa(line,b,10));
    
    hang();
}
static inline void
reboot_system(){
  for (;;){
		int i;
		for (i = 0;i < 0x10000; i++){
			if ((inb(0x64) & 0x02) == 0){
				break;
			}

		}
		outb(0x64, 0xfe);
	}
}