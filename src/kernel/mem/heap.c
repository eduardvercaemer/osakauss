#include <kernel/heap.h>
#include <kernel/log.h>
#include <kernel/paging.h>
#include <kernel/phys.h>

/* exports */

u32 heap_base = 0x80000000;

extern void
heap_init(void)
{
	// TODO: incorporate heap allocator
}

extern usize
kmalloc(u32 sz)
{
	return kmalloc__(sz, false, NULL);
}

extern usize
kmalloc_a(u32 sz)
{
	return kmalloc__(sz, true, NULL);
}

extern usize
kmalloc_ap(u32 sz, u32* phys)
{
	return kmalloc__(sz, true, phys);
}

extern usize
kmalloc__(u32 sz, bool align, u32 *phys)
{
	// for now kmaloc just allocates a whole page cause why not
	u32 ret;
	
	tracef("request for %d bytes\n", sz);
	
	u32 paddr = physmem_alloc();
	paging_kmap(paddr, heap_base);
	ret = heap_base;
	heap_base += 0x1000;
	
	tracef("> %p\n", ret);
	return ret;
}
