#pragma once
#include <types.h>
#include <kernel/ISR.h>

/*
 * A page table entry, describes a single page mapping
 */
struct page { // u32
	u32 present  : 1;
	u32 rw       : 1;
	u32 user     : 1;
	u32 accessed : 1;
	u32 dirty    : 1;
	u32 unused   : 7;
	u32 frame    : 20;
};

/*
 * A page table, holds 1024 page table entries, each describing
 * a page
 */
struct page_t { // 1024 * u32 = 0x1000 = a page
	struct page pages[1024];
};

/*
 * The page directory, an array of pointers to page tables
 */
struct page_d {
	struct page_t *tables[1024];
	u32 tables_phys[1024];
	u32 phys;
};

/* kernel/mem/paging.c */

/*
 * Setups the kernel directory, the physical memory allocator, the heap, and enables paging
 */
extern void paging_init(void);

/*
 * Load the given page directory
 */
extern void paging_switch_dir(struct page_d *dir);

/*
 * map the kernel vaddr to the specified paddr
 */
extern void paging_kmap(u32 paddr, u32 vaddr);

/*
 * Handler for page faults.
 */
extern void paging_page_fault(struct regs *r);

/*
 * TODO: dump the current status of a page directory
 */
extern void paging_dump_directory(const struct page_d *dir);