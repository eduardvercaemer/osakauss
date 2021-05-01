#include <kernel/log.h>
#include <kernel/paging.h>
#include <x86.h>
#include "frames.h"

// 8M of phys mem
#define FRAME_COUNT 0x800

// the frame bitmap for the physical memory
static struct frame_bitmap frames;
// wether the allocator is setup already
static bool physmem_ready = false;

/* linker */
extern u32 end;

/* exports */

u32 physmem_base = (u32) &end;

extern void
physmem_init(void)
{
	tracef("setting up the physmem manager\n", NULL);
	
	// create the frame map
	tracef("> allocating the bitmap at [%p]\n", physmem_base);
	frames.frames = (u32*) physmem_base;
	paging_kmap(physmem_base, physmem_base);
	physmem_base += FRAME_COUNT / 32;
	tracef("> for %d frames\n", FRAME_COUNT);
	frames.nframes = FRAME_COUNT;

	// align base
	tracef("> aligning physmem_base\n", NULL);
	if (physmem_base & 0xfff) physmem_base += 0x1000;
	physmem_base &= 0xfffff000;
	tracef("> final physmem_base at [%p]\n", physmem_base);

	physmem_ready = true;
}

extern u32
physmem_alloc(void)
{
	// early allocation, not free-able
	if (!physmem_ready) {
		if (physmem_base & 0xfff) physmem_base += 0x1000;
		physmem_base &= 0xfffff000;
		u32 ret = physmem_base;
		physmem_base += 0x1000;
		return ret;
	}

	// regular allocations
	u32 frame = frame_first(&frames);
	if (frame == FRAME_COUNT) {
		tracef("out of frames !\n", NULL);
		hang();
	}
	frame_set(&frames, frame);
	u32 paddr = physmem_base + frame * 0x1000;
	tracef("frame [%d] paddr [%p]\n", frame, paddr);
	return paddr;
}

extern void
physmem_free(u32 paddr)
{
	tracef("freeing paddr [%p]\n", paddr);
	u32 frame = (paddr - physmem_base) / 0x1000;
	if (!frame_test(&frames, frame)) {
		tracef("> double free !", NULL);
		return;
	}
	frame_clear(&frames, frame);
	tracef("> done\n", NULL);
}