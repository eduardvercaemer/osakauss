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
 * Initialize the physical memory bookkeeper, by creating its necessary
 * resources by stealing a chunk of the physmem.
 *
 * After a call to this method, physmem_base will be set to its _final_
 * value, we need to identity map this chunk of memory, and any further
 * needs for physical memory need to be satisfied via physmem_alloc
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