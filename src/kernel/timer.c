#include <kernel/IRQ.h>
#include <kernel/ISR.h>
#include <x86.h>
#include <kernel/log.h>
#include <stdlib.h>
#include <types.h>

static u32 tick = 0;

extern void 
timer_callback(regs_t *regs)
{
   /* silence unused argument warning */
   regs = regs;

   tick++;
   
    if (tick % 18 == 0)
    {
        //nothing here
    }
}

extern void 
timer_init(u32 frequency)
{
   install_handler(32, &timer_callback);
   const u32 divisor = 1193180 / frequency;
   outb(0x43, 0x36);
   const u8 l = (u8)(divisor & 0xFF);
   const u8 h = (u8)( (divisor>>8) & 0xFF );
   outb(0x40, l);
   outb(0x40, h);
} 




extern void 
timer_wait(int ticks)
{
   unsigned long eticks;
   eticks = tick + ticks;
   while(tick < eticks){
         /* 
            This is needed, If my theory is correct the compiles 
            just ignores the code and never stops the loop.
            But this code just does nothing but it atleast stops the compiler from being stupid
         */
         asm volatile("nop");
   }
}