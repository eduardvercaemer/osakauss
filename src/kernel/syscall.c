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

   int (*fnptr)(u64 a, u64 b, u64 c, u64 d, u64 e) = (void *)location;

    int ret = fnptr(r->rdi, r->rsi, r->rdx, r->rcx, r->rbx);
}

extern void
syscall_init(){
    install_handler(128, &syscall_handle);
}

