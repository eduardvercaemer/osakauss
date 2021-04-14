#pragma once
#include <types.h>

/* kernel/IDT.c */
extern void idt_set_gate(u8 num, unsigned long base, u16 sel, u8 flags);
extern void idt_init(void);