#include <kernel/heap.h>
#include <kernel/log.h>
#include "phys.h"

/* exports */

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
	if (!physmem_ready) { // only use placement allocation while physmem manager is not ready
		if (align == 1 && (placement_address & 0xfff)) { // page not already aligned
			placement_address &= 0xfffff000;
			placement_address += 0x00001000;
		}
		
		if (phys) {
			*phys = placement_address;
		}
		
		u32 tmp = placement_address;
		placement_address += sz;
		
		//tracef("[%p] %s %d bytes\n",
		//       tmp,
		//       align ? "(align)" : "",
		//       sz);
		
		return tmp;
	}
	
	tracef("unimplemented: heap allocation\n", NULL);
}
