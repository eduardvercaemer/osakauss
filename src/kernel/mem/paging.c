#include <kernel/phys.h>
#include <kernel/paging.h>
#include <kernel/heap.h>
#include <kernel/log.h>
#include <kernel/IRQ.h>
#include <stdlib.h>
#include <x86.h>

static struct page_t *table_map         = (struct page_t *) 0x40000000; // all our kernel tables at table 512
static struct page_d *kernel_directory  = (struct page_d *) 0x40400000; // kernel directory at table 513
static u32 kernel_paddr                 = 0x00000000;					// kernel directory physmem base
static struct page_d *current_directory = NULL;
static bool paging_enabled = false;

/* linker */
extern u32 end;

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

// create a kernel memory mapping
static inline void
dir_map_page(struct page_d *dir, u32 paddr, u32 vaddr)
{
	struct page_t *table, *table_paddr, *table_vaddr;
	struct page   *page;
	
	tracef("mapping [%p] to [%p]\n", vaddr, paddr);
	u32 page_idx  = (vaddr / 0x1000) % 1024;
	u32 table_idx = (vaddr / 0x1000) / 1024;
	tracef("> page %d\n", page_idx);
	tracef("> table %d\n", table_idx);
	
	// create if table not present
	if (!((u32)dir->tables[table_idx] & 1)) {		
		tracef("> allocating table %d\n", table_idx);
		table_paddr = (struct page_t *) physmem_alloc();
		// this makes so table 0 is at the first page of the table_map,
		// table 512 at the middle, etc.
		table_vaddr = &table_map[table_idx];
		tracef("> > paddr [%p]\n", table_paddr);
		tracef("> > vaddr [%p]\n", table_vaddr);
		
		// create the PDE (user rw present)
		dir->tables[table_idx] = (struct page_t *) ((u32) table_paddr | 0x7);
		
		if (paging_enabled) {
			table = table_vaddr;
		} else {
			table = table_paddr;
		}
		
		// we also need to map this table
		dir_map_page(dir, table_paddr, table_vaddr);
	} else if (paging_enabled) {
		table = &table_map[table_idx];
	} else {
		table = (struct page_t *)((u32)dir->tables[table_idx] & 0xfffff000);
	}
	
	tracef("> table on [%p]\n", table);
	page = &table->pages[page_idx];
	tracef("> page on [%p]\n", page);
	tracef("> maps to frame %x\n", paddr / 0x1000);
	map_page(page, paddr / 0x1000, 1, 1);
}


/*
 * given the paddr of a paging directory, enable it
 */
static void
paging_switch_dir_paddr(u32 dir_paddr)
{
	tracef("directory [%p]\n", dir_paddr);
	current_directory = dir_paddr;
	asm volatile("mov %0, %%cr3":: "r"(dir_paddr));
	u32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

/* exports */

extern void
paging_init(void)
{
	struct page_d *dir;
	
	tracef("setting up kernel paging\n", NULL);
	
	tracef("allocating kernel directory\n", NULL);
	kernel_paddr = physmem_alloc();
	dir = (struct page_d *) kernel_paddr; // we use the paddr since paging isn't on yet
	tracef("> > paddr [%p]\n", kernel_paddr);
	tracef("> > vaddr [%p]\n", kernel_directory);
	
	u32 kernel_frames = 1 + (u32)&end / 0x1000;
	tracef("> mapping %d kernel frames\n", kernel_frames);
	for (u32 frame = 0; frame < kernel_frames; frame++) {
		dir_map_page(dir, frame * 0x1000, frame * 0x1000);
	}

	tracef("> mapping kernel directory at [%p]\n", kernel_directory);
	dir_map_page(dir, dir, kernel_directory);
	
	//dump_directory(dir);
	//asm volatile("int $3");
	
	tracef("> installing page fault handler\n", NULL);
	install_handler(14, paging_page_fault);
	
	// Now, enable paging!
	tracef("> enabling paging\n", NULL);
	paging_switch_dir_paddr(kernel_paddr);
	paging_enabled = true;
}



/*
 * used to map kernel pages, once paging is enabled
 */
extern void
paging_kmap(u32 paddr, u32 vaddr)
{
	tracef("paddr [%p] vaddr [%p]\n", paddr, vaddr);
	
	dir_map_page(kernel_directory, paddr, vaddr);
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
		reserved ? "(reserved) " : "",
		id ? "(instruction fetch) " : "");
	
	// handle fault
	tracef("> halting\n", NULL);
	hang();
}

extern void
paging_dump_directory(struct page_d *dir)
{
	tracef("\n\n     START OF DIRECTORY DUMP\n\n", NULL);
	tracef("> paging enabled ? (%s)\n",
		paging_enabled ? "Y" : "N");
	
	for (u32 table_idx = 0; table_idx < 1024; table_idx++) {
		struct page_t *table_paddr, *table_vaddr, *table;
		table_paddr = (struct page_t *) ((u32)dir->tables[table_idx] & 0xfffff000);
		table_vaddr = &table_map[table_idx];
		if (table_paddr == NULL) continue;
		
		tracef(
			"----- t%d v[%p] p[%p]\n",
			table_idx,
			table_vaddr,
			table_paddr);
		if (paging_enabled) {
			table = table_vaddr;
		} else {
			table = table_paddr;
		}
		tracef("> addressing with [%p]\n", table);
		
		tracef("> mappings:\n", NULL);
		for (u32 page_idx = 0; page_idx < 1024; page_idx++) {
			struct page *page;
			page = &table->pages[page_idx];

			tracef("  - p%d [%p] %s%s%s%s%smaps v[%p] to p[%p]\n",
				page_idx,
				page,
				page->present ? "(p) " : "(np) ",
				page->rw ? "(rw) " : "(r) ",
				page->user ? "(user) " : "",
				page->accessed ? "(accessed) " : "",
				page->dirty ? "(dirty) " : "",
				table_idx * 0x1000 * 1024 + page_idx * 0x1000,
				page->frame * 0x1000);
		}
	}
}
