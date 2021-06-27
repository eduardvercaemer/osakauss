#ifndef __TSS_HEADER__
#define __TSS_HEADER__
#include <types.h>

//The tss code is in kernel/GDT.c

struct STSS{
    u32 reserved;
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;
    u64 reserved1;
    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;
    u64 reserved2;
    u16 reserved3;
    u16 iopb; // IOPB Offset
}TSS __attribute__((packed));


static void TSSInit();
#endif