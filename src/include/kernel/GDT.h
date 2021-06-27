#ifndef __GDT_HEADER__
#define __GDT_HEADER__
#include <types.h>

struct SGDTDescriptor {
    u16 Size;
    u64 Offset;
} __attribute__((packed));

typedef struct SGDTDescriptor GDTDescriptor;


struct SGDTEntry {
    u16 Limit0;
    u16 BaseLow;
    u8 BaseMiddle;
    u8 AccessByte;
    u8 Flags; // Limit1_flags
    u8 BaseHigh;
}__attribute__((packed));

typedef struct SGDTEntry GDTEntry;


struct SGDT {
    GDTEntry Null; //0x00
    GDTEntry KernelCode; //0x08
    GDTEntry KernelData; //0x10
    GDTEntry UserCode;
    GDTEntry UserData;
    GDTEntry TSS;
} __attribute__((packed)) 
__attribute((aligned(0x1000)));

typedef struct SGDT GDT;


extern GDT DefaultGDT;
extern void LoadGDT(GDTDescriptor* gdtDescriptor);
extern bool gdtInit();
#endif