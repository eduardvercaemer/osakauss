#pragma once
#include <types.h>

/* kernel/mem/heap.c */

#define HEAP_START 0x80000000

/*
 * you _must_ call this before any of the methods in here can
 * work properly
 */
extern i32 heap_init(void);

extern void * kmalloc(u32 sz);
extern void * kmalloc_a(u32 sz); // aligned
extern void * kmalloc_ap(u32 sz, u32* phys); // aligned
extern void * kmalloc__(u32 sz, bool align, u32 *phys); // internal method
extern void  kfree(void * ptr);