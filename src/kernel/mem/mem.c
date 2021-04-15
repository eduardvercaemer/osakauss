#include <kernel/mem.h>
#include <kernel/log.h>

/* linker */
extern u32 end;
u32 placement_address = NULL;

/* exports */

extern void
kheap_init(void)
{
	placement_address = (u32)&end;
	tracef("[%p]\n", placement_address);
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
	if (align == 1 && (placement_address & 0xfff)) { // page not already aligned
		placement_address &= 0xfffff000;
		placement_address += 0x00001000;
	}
	
	if (phys) {
		*phys = placement_address;
	}
	
	u32 tmp = placement_address;
	placement_address += sz;
	
	tracef("[%p] %s %d bytes\n",
	     tmp,
	     align ? "(align)" : "",
	     sz);
	
	return tmp;
}
