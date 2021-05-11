#pragma once

typedef struct {
    unsigned int gs, fs, es, ds;     
    unsigned int rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;  
    unsigned int int_no, err_code;    
    unsigned int eip, cs, eflags, useresp, ss;   
} regs_t;

/* kernel/IDT.c */
extern void isr_init(void);