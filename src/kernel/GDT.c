#include <kernel/GDT.h>
#include <kernel/TSS.h>

__attribute__((aligned(0x1000)))

GDT DefaultGDT = {
    {0, 0, 0, 0x00, 0x00, 0}, // null
    {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
    {0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
    {0, 0, 0, 0xFa, 0xa0, 0}, // user code segment
    {0, 0, 0, 0xF2, 0xa0, 0}, // user data segment
    {sizeof(TSS), 0, 0, 0x89, 0x40,} // TSS
};

GDTDescriptor gdtDescriptor = {};

static void setupTSS(){
    u64 base = (u64) &TSS;
    DefaultGDT.TSS.BaseLow    = (base & 0xFFFF);
    DefaultGDT.TSS.BaseMiddle = (base >> 16) & 0xFF;
    DefaultGDT.TSS.BaseHigh   = (base >> 24) & 0xFF;
    DefaultGDT.TSS.Limit0 = sizeof(TSS);
    DefaultGDT.TSS.AccessByte = 0x89;
    DefaultGDT.TSS.Flags = 0x40;
}


extern bool gdtInit(){ // also loads the tss
    setupTSS();

    TSS.iopb = sizeof(TSS);
    TSS.rsp0 = 0x0;


    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (u64)&DefaultGDT;
    LoadGDT(&gdtDescriptor);
    TssFlush();
    return true;
}