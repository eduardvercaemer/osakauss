#pragma once
#include <types.h>

/* kernel/mem/heap.c */

extern u32 heap_base;

/*
 * you _must_ call this before any of the methods in here can
 * work properly
 */
extern void heap_init(void);

extern usize kmalloc(u32 sz);
extern usize kmalloc_a(u32 sz); // aligned
extern usize kmalloc_ap(u32 sz, u32* phys); // aligned
extern usize kmalloc__(u32 sz, bool align, u32 *phys); // internal method