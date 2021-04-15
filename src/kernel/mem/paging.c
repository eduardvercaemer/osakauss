#include <kernel/heap.h>
#include <kernel/paging.h>
#include <kernel/log.h>
#include <stdlib.h>
#include <x86.h>
#include "phys.h"

static struct page_d *kernel_directory = NULL;
static struct page_d *current_directory = NULL;

/* static methods */

static void
map_page(struct page *page, u32 frame, bool is_kernel, bool is_writeable)
{
	page->present = 1; // Mark it as present.
	page->rw      = is_writeable ? 1 : 0; // Should the page be writeable?
	page->user    = is_kernel    ? 0 : 1; // Should the page be user-mode?
	page->frame   = frame;
}


// Performs the identiy mapping of the current kernel image
static struct page_d *
create_identity_dir(void)
{
	tracef("creating kernel identity-map\n", NULL);
	//tracef("> allocating page directory\n", NULL);
	struct page_d *dir = (struct page_d *) kmalloc_a(sizeof *dir);
	memset(dir, 0, sizeof *dir);
	//tracef("> building dir in [%p]\n", dir);
	
	u32 addr = 0;
	while (addr < placement_address) {
		//tracef("> mapping [%p]\n", addr);
		map_page(paging_get_page(addr, 1, dir), addr/0x1000, 1, 1);
		addr += 0x1000;
	}
	//tracef("> mapped %d frames; [%p] -> [%p]\n", addr/0x1000, 0, addr - 1);
	
	return dir;
}

/* exports */

extern void
paging_init(void)
{
	// create the physmem manager
	physmem_init();
	
	tracef("setting up kernel paging\n", NULL);
	kernel_directory = create_identity_dir();
	// !! no more kmalloc until heap is setup !!
	
	physmem_start();
	
	// Now, enable paging!
	tracef("enabling paging\n", NULL);
	paging_switch_dir(kernel_directory);
}

extern void
paging_switch_dir(struct page_d *dir)
{
	tracef("directory [%p]\n", dir);
	current_directory = dir;
	asm volatile("mov %0, %%cr3":: "r"(&dir->tables_phys));
	u32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

extern struct page *
paging_get_page(usize address, bool make, struct page_d *dir)
{
	//tracef("vaddr [%p] from dir [%p] %s\n",
	//	address,
	//	dir,
	//	make ? "(make)" : "");
	// Turn the address into an index.
	address /= 0x1000;
	// Find the page table containing this address.
	u32 table_idx = address / 1024;
	//tracef("> table %d\n", table_idx);
	
	if (dir->tables[table_idx]) {
		struct page *p = &dir->tables[table_idx]->pages[address%1024];
		//tracef("> page entry at [%p]\n", p);
		return p;
	} else if (make) {
		u32 tmp;
		dir->tables[table_idx] = (struct page_t*) kmalloc_ap(sizeof(struct page_t), &tmp);
		//tracef("> made table vaddr [%p] paddr [%p]\n",
		//  dir->tables[table_idx],
		//  tmp);
		memset(dir->tables[table_idx], 0, 0x1000);
		dir->tables_phys[table_idx] = tmp | 0x7; // PRESENT, RW, US.
		struct page *p = &dir->tables[table_idx]->pages[address%1024];
		//tracef("> page entry at [%p]\n", p);
		return p;
	} else {
		tracef("> missing table, not making it !\n", NULL);
		return 0;
	}
}

extern void
paging_page_fault(struct regs *r)
{
	// A page fault has occurred.
	// The faulting address is stored in the CR2 register.
	u32 faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	
	// The error code gives us details of what happened.
	bool not_present = !(r->err_code & 0x1);   // Page not present
	bool write       = r->err_code & 0x2;      // Write operation?
	bool user        = r->err_code & 0x4;      // Processor was in user-mode?
	bool reserved    = r->err_code & 0x8;      // Overwritten CPU-reserved bits of page entry?
	bool id          = r->err_code & 0x10;     // Caused by an instruction fetch?
	
	// Output an error message.
	tracef("[%p] %s%s%s%s\n",
		faulting_address,
		not_present ? "(not present) " : "",
		write ? "(write) " : "(read) ",
		user ? "(user-mode) " : "",
		reserved ? "(reserved) " : "");
	
	// handle fault
	tracef("> halting\n", NULL);
	hang();
}
