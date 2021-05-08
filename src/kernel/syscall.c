#include <kernel/syscall.h>
#include <types.h>
#include <kernel/ISR.h>
#include <kernel/IRQ.h>
#include <kernel/IDT.h>
#include <kernel/log.h>
#include <stdlib.h>

#define num_syscalls 1

/*
 * Example syscall, normally, you would simply need to include the header
 * defining the function and the implementation could go somewhere else
 */
static int
log_best_number_ever(void) {
    tracef("hello from a syscall !", NULL);
    return 0;
}

/*
This is where you define your syscall. 
There is also a part where you need to define in src/include/syscall.h
*/
DEFN_SYSCALL0(log_best_number_ever, 0);

/*
This is a list of all the system functions that are going to be used as a system call
*/
static void *
SYS_CALLS[num_syscalls] = 
{
    &log_best_number_ever,
};

extern void 
syscall_handle(regs_t *r){
    if (r->int_no >= num_syscalls)
       return;

   // Get the required syscall location.
   void *location = SYS_CALLS[r->int_no];

   // We don't know how many parameters the function wants, so we just
   // push them all onto the stack in the correct order. The function will
   // use all the parameters it wants, and we can pop them all back off afterwards.
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

extern void
syscall_init(){
    install_handler(128, &syscall_handle);
}

