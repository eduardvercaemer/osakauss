#include <kernel/mem.h>
#include <kernel/log.h>
#include <stdlib.h>
#include <x86.h>

static struct page_d *kernel_directory = NULL;
static struct page_d *current_directory = NULL;

/* frame allocation */

static u32 *frames; // bitmap
static u32 nframes; // size of bitmap

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Set a bit in the frames bitset
static void
set_frame(u32 frame_addr)
{
	u32 frame = frame_addr/0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
	
	//tracef("set [%d]\n", frame);
}

// Clear a bit in the frames bitset
static void
clear_frame(u32 frame_addr)
{
	u32 frame = frame_addr/0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
	
	//tracef("clear [%d]\n", frame);
}

// Test if a bit is set.
static bool
test_frame(u32 frame_addr)
{
	u32 frame = frame_addr/0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	bool ret = frames[idx] & (0x1 << off);
	
	//tracef("test [%d]: %s\n", frame, ret ? "set" : "clear");
	
	return ret;
}

// Find the first free frame.
static u32
first_frame(void)
{
	u32 i, j;
	u32 frame;
	for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] == 0xffffffff) { // nothing free in here
			continue;
		}
		
		for (j = 0; j < 32; j++) {
			u32 toTest = 0x1 << j;
			if ( !(frames[i] & toTest) ) {
				frame = i*4*8+j;
				goto done;
			}
		}
	}
	
	// nothing found !
	tracef("%s", "no empty frame!\n");
	hang();
	
	done:
	//logf("%s: empty frame at %d\n", __func__, frame);
	return frame;
}

// Allocate a frame.
static void
alloc_frame(struct page *page, bool is_kernel, bool is_writeable)
{
	//tracef("allocating frame %s%s\n", is_kernel ? "(kernel) " : "", is_writeable ? "(rw)" : "(r)");
	//tracef("> page [%p]\n", (u32)page);
	if (page->frame != 0) {
		//tracef("%s", "> already allocated\n");
		return; // Frame was already allocated, return straight away.
	} else {
		//tracef("%s", "> requesting free frame\n");
		u32 idx = first_frame(); // idx is now the index of the first free frame.
		if (idx == 0xffffffff) {
			logf("panic: out of memory frames!\n");
			hang();
		}
	//	tracef("> frame [%d] at [%p]\n", idx, idx * 0x1000);
		
		set_frame(idx*0x1000); // this frame is now ours!
		page->present = 1; // Mark it as present.
		page->rw      = is_writeable ? 1 : 0; // Should the page be writeable?
		page->user    = is_kernel    ? 0 : 1; // Should the page be user-mode?
		page->frame   = idx;
		//tracef("phys [%p]\n", idx*0x1000);
	}
	
	//tracef("%s", "> done\n");
}

// Deallocate a frame.
static void
free_frame(struct page *page)
{
	u32 frame;
	if (!(frame=page->frame)) {
		return; // The given page didn't actually have an allocated frame!
	} else {
		clear_frame(frame); // Frame is now free again.
		page->frame = 0x0; // Page now doesn't have a frame.
	}
}

/* exports */

extern void
paging_init(void)
{
	// The size of physical memory. For the moment we
	// assume it is 16MB big.
	u32 mem_end_page = 0x1000000;

	nframes = mem_end_page / 0x1000;
	tracef("allocating frame bitmap (%d frames)\n", nframes);
	frames = (u32*)kmalloc(INDEX_FROM_BIT(nframes) * sizeof(u32));
	memset(frames, 0, INDEX_FROM_BIT(nframes));
	
	tracef("%s\n", "allocating kernel directory");
	kernel_directory = (struct page_d *) kmalloc_a(sizeof(struct page_d));
	memset(kernel_directory, 0, sizeof(struct page_d));
	current_directory = kernel_directory;
	tracef("building kernel directory at [%p]\n", kernel_directory);
	
	// We need to identity map (phys addr = virt addr) from
	// 0x0 to the end of used memory, so we can access this
	// transparently, as if paging wasn't enabled.
	// NOTE that we use a while loop here deliberately.
	// inside the loop body we actually change placement_address
	// by calling kmalloc(). A while loop causes this to be
	// computed on-the-fly rather than once at the start.
	u32 i = 0;
	tracef("identity map kernel\n", NULL);
	while (i < placement_address) {
		// Kernel code is readable but not writeable from userspace.
		//tracef("> mapping [%p]\n", i);
		alloc_frame( paging_get_page(i, 1, kernel_directory), 1, 0);
		i += 0x1000;
	}
	// Before we enable paging, we must register our page fault handler.
	//register_interrupt_handler(14, page_fault);
	
	// Now, enable paging!
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
	for (;;);
}
