#pragma once
#include <types.h>
/*
 * the physical memory allocator
 */

/*
 * base of the managed phys. memory
 */
extern u32 physmem_base;

/*
 * initialize the physical memory bookkeeper, by creating its necessary resources
 */
extern void physmem_init(void);

/*
 * request the address of phys. page from the allocator
 */
extern u32 physmem_alloc(void);

/*
 * free a phys. page from the allocator
 */
extern void physmem_free(u32 paddr);