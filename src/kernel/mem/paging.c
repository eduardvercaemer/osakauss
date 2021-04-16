#include <kernel/phys.h>
#include <kernel/paging.h>
#include <kernel/log.h>
#include <stdlib.h>
#include <x86.h>

static struct page_d *kernel_directory = NULL;
static struct page_d *current_directory = NULL;

/* static methods */

// given a page entry, make it point to the given physical frame
static void
map_page(struct page *page, u32 frame, bool is_kernel, bool is_writeable)
{
	page->present = 1; // Mark it as present.
	page->rw      = is_writeable ? 1 : 0; // Should the page be writeable?
	page->user    = is_kernel    ? 0 : 1; // Should the page be user-mode?
	page->frame   = frame;
}

/* exports */

extern void
paging_init(void)
{
	tracef("setting up kernel paging\n", NULL);
	
	tracef("> aligning physmem_base\n", NULL);
	if (physmem_base & 0xfff) physmem_base += 0x1000;
	physmem_base &= 0xfffff000;
	
	tracef("> allocating kernel directory at [%p]\n", physmem_base);
	kernel_directory = (struct page_d *) physmem_base;
	physmem_base += sizeof *kernel_directory;
	memset(kernel_directory, 0, sizeof *kernel_directory);
	
	tracef("> aligning physmem_base\n", NULL);
	if (physmem_base & 0xfff) physmem_base += 0x1000;
	physmem_base &= 0xfffff000;
	
	tracef("> building kernel directory\n", NULL);
	u32 addr = 0;
	while (addr < physmem_base) { // identity map early kernel
		u32 page_idx  = (addr / 0x1000) % 1024;
		u32 table_idx = (addr / 0x1000) / 1024;
		struct page_t *table = kernel_directory->tables[table_idx];
		if (!table) {
			tracef("> allocating page table index %d at [%p]\n", table_idx, physmem_base);
			table = (struct page_t *) physmem_base;
			kernel_directory->tables[table_idx] = table;
			kernel_directory->tables_phys[table_idx] = physmem_base | 0x7;
			memset(table, 0, 0x1000);
			physmem_base += 0x1000;
		}
		struct page *page = &table->pages[page_idx];
		tracef("> mapping [%p]\n", addr);
		map_page(page, addr/0x1000, 1, 1);
		addr += 0x1000;
	}
	
	// Now, enable paging!
	tracef("> enabling paging\n", NULL);
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

extern void
paging_kmap(u32 paddr, u32 vaddr)
{
	tracef("paddr [%p] vaddr [%p]\n", paddr, vaddr);
	
	u32 page_idx  = (vaddr / 0x1000) % 1024;
	u32 table_idx = (vaddr / 0x1000) / 1024;
	tracef("> on table %d\n", table_idx);
	tracef("> on index %d\n", page_idx);
	
	struct page_t *table = kernel_directory->tables[table_idx];
	if (!table) {
		table = (struct page_t *) physmem_alloc();
		tracef("> allocating new page table at [%p]\n", table);
		kernel_directory->tables[table_idx] = table;
		kernel_directory->tables_phys[table_idx] = ((u32) table) | 0x7;
		memset(table, 0, 0x1000);
	}
	
	struct page *page = &table->pages[page_idx];
	map_page(page, paddr/0x1000, 1, 1);
	tracef("> mapping on page at [%p]\n", page);
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
