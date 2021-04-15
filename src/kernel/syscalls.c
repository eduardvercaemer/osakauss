#include <x86.h>
#include <kernel/log.h>
#include <stdlib.h>
#include <types.h>
#include <kernel/syscall.h>
#include <kernel/ISR.h>



static void syscall_handler(regs *r);




DEFN_SYSCALL1(logf, 0, const char*);





#define num_syscalls 1

static void *
SYS_CALLS[num_syscalls] = 
{
    0,
    &logf,
};



extern void 
syscalls_init()
{
   install_handler(128, &syscall_handler);
} 








extern void 
syscall_handler(regs *r)
{
    logf("TYo");
   if (r->eax >= num_syscalls)
       return;
   
   void *location = SYS_CALLS[r->eax];


   int ret;
   asm volatile (" \
     push %1; \
     push %2; \
     push %3; \
     push %4; \
     push %5; \
     call *%6; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
     pop %%ebx; \
   " : "=a" (ret) : "r" (r->edi), "r" (r->esi), "r" (r->edx), "r" (r->ecx), "r" (r->ebx), "r" (location));
   r->eax = ret;
} 



