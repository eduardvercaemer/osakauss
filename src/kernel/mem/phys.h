#pragma once
#include <types.h>
/*
 * the physical memory allocator
 */

/*
 * wether the manager has been started or not
 */
extern bool physmem_ready;

/*
 * initialize the physical memory bookkeeper, by creating its necessary resources
 */
extern void physmem_init(void);

/*
 * _start_ the phys mem bookkeeper, by giving it the base of the phys mem
 */
extern void physmem_start(void);

/*
 * request the address of phys. page from the allocator
 */
extern u32 physmem_alloc(void);

/*
 * free a phys. page from the allocator
 */
extern void physmem_free(u32 paddr);