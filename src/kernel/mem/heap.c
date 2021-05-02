#include <types.h>
#include <kernel/heap.h>
#include <kernel/log.h>
#include <kernel/paging.h>
#include <kernel/phys.h>

u32 heap_base = 0x80000000; // where the heap starts
u32 heap_head = 0x80000000; // where the heap head currently is
u32 heap_top  = 0x80000000; // where the mapped heap ends

/* exports */

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
	usize ret;

	if (align) {
		if (heap_head & 0xfff) heap_head += 0x1000;
		heap_head &= 0xfffff000;
	}

	ret = heap_head;
	heap_head += sz;

	while (heap_head > heap_top) {
		u32 paddr = physmem_alloc();
		paging_kmap(paddr, heap_top);
		heap_top += 0x1000;
	}

	if (phys) {
		u32 frame;
		paging_vaddr_get_kmap(ret, &frame);
		*phys = frame * 0x1000;
	}

	return ret;
}
