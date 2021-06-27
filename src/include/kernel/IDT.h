#ifndef __IDT_HEADER__
#define __IDT_HEADER__
#include <types.h>

struct IDT{
  u16 offset_low;
  u16 selector;
  u8 ist;
  u8 types_attr;
  u16 offset_mid;
  u32 offset_high;
  u32 reserved;
}__attribute__((packed));

struct IDTPtr {
    u16 limit;
    u64 base;
}__attribute((packed));

bool IDTInit();
#endif