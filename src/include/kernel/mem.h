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

/* kernel/mem/kheap.c */

/*
 * Initialize the heap, must call this before any ohter methods
 */
extern void kheap_init(void);

extern u32 placement_address;
extern usize kmalloc(u32 sz);
extern usize kmalloc_a(u32 sz); // aligned
extern usize kmalloc_ap(u32 sz, u32* phys); // aligned
extern usize kmalloc__(u32 sz, bool align, u32 *phys);

/* kernel/mem/paging.c */

/*
 * Setups and enables paging
 */
extern void paging_init(void);

/*
 * Load the given page directory
 */
extern void paging_switch_dir(struct page_d *dir);


/*
 * Get a pointer to the page entry that maps the given vaddr given,
 * if the table for this entry is missing, and we set make, this
 * will create the needed table for the page
 */
extern struct page *paging_get_page(usize address, bool make, struct page_d *dir);

/*
 * Handler for page faults.
 */
extern void paging_page_fault(struct regs *r);
