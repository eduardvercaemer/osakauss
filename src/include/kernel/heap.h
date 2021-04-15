#pragma once
#include <types.h>

/* kernel/mem/heap.c */

extern u32 placement_address;
extern usize kmalloc(u32 sz);
extern usize kmalloc_a(u32 sz); // aligned
extern usize kmalloc_ap(u32 sz, u32* phys); // aligned
extern usize kmalloc__(u32 sz, bool align, u32 *phys); // internal method