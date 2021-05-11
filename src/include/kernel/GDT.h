#pragma once
#include <types.h>

struct SGDTDescriptor {
    u16 Size;
    u64 Offset;
} __attribute__((packed));

typedef struct SGDTDescriptor GDTDescriptor;

struct SGDTEntry {
    u16 Limit0;
    u16 Base0;
    u8 Base1;
    u8 AccessByte;
    u8 Limit1_Flags;
    u8 Base2;
}__attribute__((packed));

typedef struct SGDTEntry GDTEntry;


struct SGDT {
    GDTEntry Null; //0x00
    GDTEntry KernelCode; //0x08
    GDTEntry KernelData; //0x10
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;
} __attribute__((packed)) 
__attribute((aligned(0x1000)));

typedef struct SGDT GDT;


extern GDT DefaultGDT;
extern void LoadGDT(GDTDescriptor* gdtDescriptor);



/*

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed));

struct gdt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

*/

