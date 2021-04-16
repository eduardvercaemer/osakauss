

#include <types.h>
#include <kernel/ISR.h>
#include <kernel/IDT.h>
#include <kernel/log.h>


extern void syscall_handle(regs *r){
    logf("a");
}

extern void
syscall_init(){
    install_handler(128,&syscall_handle);
}